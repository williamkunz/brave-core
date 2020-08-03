/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_PLAYLIST_BROWSER_PLAYLIST_MEDIA_FILE_DOWNLOADER_H_
#define BRAVE_COMPONENTS_PLAYLIST_BROWSER_PLAYLIST_MEDIA_FILE_DOWNLOADER_H_

#include <list>
#include <memory>
#include <string>
#include <vector>

#include "base/files/file_path.h"
#include "base/macros.h"
#include "base/memory/weak_ptr.h"
#include "base/values.h"

namespace base {
class FilePath;
class SequencedTaskRunner;
}  // namespace base

namespace content {
class BrowserContext;
}  // namespace content

namespace network {
class SharedURLLoaderFactory;
class SimpleURLLoader;
}  // namespace network

class GURL;

namespace playlist {

// Handle one Playlist at once.
class PlaylistMediaFileDownloader {
 public:
  class Client {
   public:
    // Called when target media file generation succeed.
    virtual void OnMediaFileReady(base::Value&& playlist_value,
                                  bool partial) = 0;
    // Called when target media file generation failed.
    virtual void OnMediaFileGenerationFailed(base::Value&& playlist_value) = 0;

   protected:
    virtual ~Client() {}
  };

  PlaylistMediaFileDownloader(
      Client* client,
      content::BrowserContext* context,
      base::FilePath::StringType source_media_files_dir,
      base::FilePath::StringType unified_media_file_name,
      std::string media_file_path_key,
      std::string create_params_path_key);
  virtual ~PlaylistMediaFileDownloader();

  void GenerateSingleMediaFile(base::Value&& playlist_value,
                               const base::FilePath& base_dir);

  void RequestCancelCurrentPlaylistGeneration();

  bool in_progress() const { return in_progress_; }
  std::string current_playlist_id() const { return current_playlist_id_; }

 private:
  using SimpleURLLoaderList =
      std::list<std::unique_ptr<network::SimpleURLLoader>>;

  void ResetStatus();
  void DownloadAllMediaFileSources();
  void DownloadMediaFile(const GURL& url, int index);
  void CreateSourceFilesDirThenDownloads();
  void OnSourceFilesDirCreated(bool success);
  void OnMediaFileDownloaded(SimpleURLLoaderList::iterator iter,
                             int index,
                             base::FilePath path);
  void StartSingleMediaFileGeneration();
  // See the comments of DoGenerateSingleMediaFileOnIOThread() about
  // the meaning of |result|.
  void OnSingleMediaFileGenerated(int result);

  // True when all source media files are downloaded.
  // If it's true, single media file will be generated.
  bool IsDownloadFinished();
  int GetNumberOfMediaFileSources();

  void NotifyFail();
  // If |partial| is true, not all source files are used for media file
  // generation for some reason.
  void NotifySucceed(bool partial);

  base::SequencedTaskRunner* io_task_runner();

  Client* client_;
  scoped_refptr<network::SharedURLLoaderFactory> url_loader_factory_;

  base::FilePath::StringType source_media_files_dir_;
  base::FilePath::StringType unified_media_file_name_;
  std::string media_file_path_key_;
  std::string create_params_path_key_;

  // All below variables are only for playlist creation.
  base::FilePath playlist_dir_path_;
  base::Value current_playlist_;
  std::string current_playlist_id_;
  int remained_download_files_ = 0;
  int media_file_source_files_count_ = 0;

  // true when this class is working for playlist now.
  bool in_progress_ = false;

  // true when user deletes currently working playlist.
  // If true, this doesn't notify to Client after finishing media file
  // generation.
  bool cancelled_ = false;

  scoped_refptr<base::SequencedTaskRunner> io_task_runner_;

  SimpleURLLoaderList url_loaders_;

  base::WeakPtrFactory<PlaylistMediaFileDownloader> weak_factory_;

  DISALLOW_COPY_AND_ASSIGN(PlaylistMediaFileDownloader);
};

base::FilePath::StringType GetPlaylistIDDirName(const std::string& playlist_id);

}  // namespace playlist

#endif  // BRAVE_COMPONENTS_PLAYLIST_BROWSER_PLAYLIST_MEDIA_FILE_DOWNLOADER_H_
