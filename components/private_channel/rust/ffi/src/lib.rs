#![allow(clippy::missing_safety_doc)]
extern crate brave_private_channel;

use std::convert::TryInto;
use std::ffi::CStr;
use std::os::raw::{c_char, c_int};
use std::slice;

pub const KEY_SIZE: usize = 32;

macro_rules! assert_not_null {
    ($var:expr) => {
        assert!(!$var.is_null(), "{} is NULL", stringify!($var));
    };
}

#[repr(C)]
pub struct ResultChallenge {
    pub pkey_ptr: *const u8,
    pub skey_ptr: *const u8,
    pub key_size: usize,
    pub shared_pubkey_ptr: *const u8,
    pub encrypted_hashes_ptr: *const u8,
    pub encrypted_hashes_size: usize,
    pub error: bool,
}

impl Default for ResultChallenge {
    fn default() -> ResultChallenge {
        let mock_vec = vec![];
        ResultChallenge {
            error: true,
            pkey_ptr: mock_vec.as_ptr(),
            skey_ptr: mock_vec.as_ptr(),
            key_size: 0,
            shared_pubkey_ptr: mock_vec.as_ptr(),
            encrypted_hashes_size: 0,
            encrypted_hashes_ptr: mock_vec.as_ptr(),
        }
    }
}

#[repr(C)]
pub struct ResultSecondRound {
    pub encoded_partial_dec_ptr: *const u8,
    pub encoded_partial_dec_size: usize,
    pub encoded_proofs_ptr: *const u8,
    pub encoded_proofs_size: usize,
    pub random_vec_ptr: *const u8,
    pub random_vec_size: usize,
    pub error: bool,
}

impl Default for ResultSecondRound {
    fn default() -> ResultSecondRound {
        let mock_vec = vec![];
        ResultSecondRound {
            error: true,
            encoded_partial_dec_ptr: mock_vec.as_ptr(),
            encoded_partial_dec_size: 0,
            encoded_proofs_ptr: mock_vec.as_ptr(),
            encoded_proofs_size: 0,
            random_vec_ptr: mock_vec.as_ptr(),
            random_vec_size: 0,
        }
    }
}

/// Starts client attestation challenge;
#[no_mangle]
pub unsafe extern "C" fn client_start_challenge(
    input: *const *const c_char,
    input_size: c_int,
    server_pk_encoded: *const u8,
) -> ResultChallenge {
    assert!(!input.is_null(), "Null pointers passed as input");
    assert!(
        !server_pk_encoded.is_null(),
        "Null pointers passed as input"
    );

    let server_pk = match slice::from_raw_parts(server_pk_encoded, KEY_SIZE).try_into() {
        Ok(pk) => pk,
        Err(_) => {
            return ResultChallenge::default();
        }
    };

    let mut v_out = Vec::new();
    let input_array = slice::from_raw_parts(input, input_size as usize);
    for n in 0..input_size {
        let s_ptr = CStr::from_ptr(input_array[n as usize]);
        v_out.push(s_ptr.to_str().unwrap().to_string());
    }

    let brave_private_channel::FirstRoundOutput {
        pkey,
        skey,
        shared_pk,
        enc_hashes,
    } = match brave_private_channel::start_challenge(v_out, server_pk) {
        Ok(result) => result,
        Err(_) => return ResultChallenge::default(),
    };

    let mut pkey_buff = pkey.into_boxed_slice();
    let pkey_ptr = pkey_buff.as_mut_ptr();
    std::mem::forget(pkey_buff);

    let mut skey_buff = skey.into_boxed_slice();
    let skey_ptr = skey_buff.as_mut_ptr();
    std::mem::forget(skey_buff);

    let mut shared_pk_buff = shared_pk.into_boxed_slice();
    let shared_pk_ptr = shared_pk_buff.as_mut_ptr();
    std::mem::forget(shared_pk_buff);

    let mut enc_hashes_buff = enc_hashes.into_boxed_slice();
    let enc_hashes_size = enc_hashes_buff.clone().len();
    let enc_hashes_ptr = enc_hashes_buff.as_mut_ptr();
    std::mem::forget(enc_hashes_buff);

    ResultChallenge {
        pkey_ptr,
        skey_ptr,
        key_size: KEY_SIZE,
        shared_pubkey_ptr: shared_pk_ptr,
        encrypted_hashes_size: enc_hashes_size,
        encrypted_hashes_ptr: enc_hashes_ptr,
        error: false,
    }
}

#[no_mangle]
pub unsafe extern "C" fn client_second_round(
    input: *const u8,
    input_size: c_int,
    client_sk_encoded: *const u8,
) -> ResultSecondRound {
    assert!(!input.is_null(), "Null pointers passed as input");
    assert!(
        !client_sk_encoded.is_null(),
        "Null pointers passed as input"
    );

    let skey_buff = slice::from_raw_parts(client_sk_encoded, KEY_SIZE as usize);

    let v_enc = slice::from_raw_parts(input, input_size as usize);

    let brave_private_channel::SecondRoundOutput {
        partial_dec,
        proofs,
        rand_vec,
    } = match brave_private_channel::second_round(v_enc, skey_buff) {
        Ok(result) => result,
        Err(_) => return ResultSecondRound::default(),
    };

    let mut partial_enc_buff = partial_dec.into_boxed_slice();
    let partial_enc_size = partial_enc_buff.len();
    let partial_enc_ptr = partial_enc_buff.as_mut_ptr();
    std::mem::forget(partial_enc_buff);

    let mut proofs_buff = proofs.into_boxed_slice();
    let proofs_size = proofs_buff.len();
    let proofs_ptr = proofs_buff.as_mut_ptr();
    std::mem::forget(proofs_buff);

    let mut rand_vec_buff = rand_vec.into_boxed_slice();
    let rand_vec_size = rand_vec_buff.len();
    let rand_vec_ptr = rand_vec_buff.as_mut_ptr();
    std::mem::forget(rand_vec_buff);

    ResultSecondRound {
        encoded_partial_dec_ptr: partial_enc_ptr,
        encoded_partial_dec_size: partial_enc_size,
        encoded_proofs_ptr: proofs_ptr,
        encoded_proofs_size: proofs_size,
        random_vec_ptr: rand_vec_ptr,
        random_vec_size: rand_vec_size,
        error: false,
    }
}

// By reconstructing the fileds of the structure in Rust and letting it out of scope,
// the Rust compiler will deallocate the memory contents
#[no_mangle]
pub unsafe extern "C" fn deallocate_first_round_result(result: ResultChallenge) {
    assert_not_null!(result.skey_ptr);
    let _key = std::slice::from_raw_parts(result.pkey_ptr, KEY_SIZE);

    assert_not_null!(result.skey_ptr);
    let _skey = std::slice::from_raw_parts(result.pkey_ptr, KEY_SIZE);

    assert_not_null!(result.shared_pubkey_ptr);
    let _share_key = std::slice::from_raw_parts(result.shared_pubkey_ptr, KEY_SIZE);

    assert_not_null!(result.encrypted_hashes_ptr);
    let _enc_hashes =
        std::slice::from_raw_parts(result.encrypted_hashes_ptr, result.encrypted_hashes_size);
}

#[no_mangle]
pub unsafe extern "C" fn deallocate_second_round_result(result: ResultSecondRound) {
    assert_not_null!(result.encoded_partial_dec_ptr);
    let _key = std::slice::from_raw_parts(
        result.encoded_partial_dec_ptr,
        result.encoded_partial_dec_size,
    );

    assert_not_null!(result.encoded_proofs_ptr);
    let _skey = std::slice::from_raw_parts(result.encoded_proofs_ptr, result.encoded_proofs_size);

    assert_not_null!(result.random_vec_ptr);
    let _share_key = std::slice::from_raw_parts(result.random_vec_ptr, result.random_vec_size);
}
