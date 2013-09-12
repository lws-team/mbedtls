/**
 * \file x509.h
 *
 * \brief X.509 certificate and private key decoding
 *
 *  Copyright (C) 2006-2013, Brainspark B.V.
 *
 *  This file is part of PolarSSL (http://www.polarssl.org)
 *  Lead Maintainer: Paul Bakker <polarssl_maintainer at polarssl.org>
 *
 *  All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifndef POLARSSL_X509_H
#define POLARSSL_X509_H

#include "config.h"

#if defined(POLARSSL_X509_PARSE_C) || defined(POLARSSL_X509_WRITE_C)
#include "asn1.h"
#include "dhm.h"
#include "md.h"
#include "pk.h"

/** 
 * \addtogroup x509_module
 * \{ 
 */
 
/** 
 * \name X509 Error codes
 * \{
 */
#define POLARSSL_ERR_X509_FEATURE_UNAVAILABLE              -0x2080  /**< Unavailable feature, e.g. RSA hashing/encryption combination. */
#define POLARSSL_ERR_X509_CERT_INVALID_PEM                 -0x2100  /**< The PEM-encoded certificate contains invalid elements, e.g. invalid character. */ 
#define POLARSSL_ERR_X509_CERT_INVALID_FORMAT              -0x2180  /**< The certificate format is invalid, e.g. different type expected. */
#define POLARSSL_ERR_X509_CERT_INVALID_VERSION             -0x2200  /**< The certificate version element is invalid. */
#define POLARSSL_ERR_X509_CERT_INVALID_SERIAL              -0x2280  /**< The serial tag or value is invalid. */
#define POLARSSL_ERR_X509_CERT_INVALID_ALG                 -0x2300  /**< The algorithm tag or value is invalid. */
#define POLARSSL_ERR_X509_CERT_INVALID_NAME                -0x2380  /**< The name tag or value is invalid. */
#define POLARSSL_ERR_X509_CERT_INVALID_DATE                -0x2400  /**< The date tag or value is invalid. */
#define POLARSSL_ERR_X509_CERT_INVALID_PUBKEY              -0x2480  /**< The pubkey tag or value is invalid (only RSA and EC are supported). */
#define POLARSSL_ERR_X509_CERT_INVALID_SIGNATURE           -0x2500  /**< The signature tag or value invalid. */
#define POLARSSL_ERR_X509_CERT_INVALID_EXTENSIONS          -0x2580  /**< The extension tag or value is invalid. */
#define POLARSSL_ERR_X509_CERT_UNKNOWN_VERSION             -0x2600  /**< Certificate or CRL has an unsupported version number. */
#define POLARSSL_ERR_X509_CERT_UNKNOWN_SIG_ALG             -0x2680  /**< Signature algorithm (oid) is unsupported. */
#define POLARSSL_ERR_X509_UNKNOWN_PK_ALG                   -0x2700  /**< Key algorithm is unsupported (only RSA and EC are supported). */
#define POLARSSL_ERR_X509_CERT_SIG_MISMATCH                -0x2780  /**< Certificate signature algorithms do not match. (see \c ::x509_cert sig_oid) */
#define POLARSSL_ERR_X509_CERT_VERIFY_FAILED               -0x2800  /**< Certificate verification failed, e.g. CRL, CA or signature check failed. */
#define POLARSSL_ERR_X509_KEY_INVALID_VERSION              -0x2880  /**< Unsupported RSA key version */
#define POLARSSL_ERR_X509_KEY_INVALID_FORMAT               -0x2900  /**< Invalid RSA key tag or value. */
#define POLARSSL_ERR_X509_CERT_UNKNOWN_FORMAT              -0x2980  /**< Format not recognized as DER or PEM. */
#define POLARSSL_ERR_X509_INVALID_INPUT                    -0x2A00  /**< Input invalid. */
#define POLARSSL_ERR_X509_MALLOC_FAILED                    -0x2A80  /**< Allocation of memory failed. */
#define POLARSSL_ERR_X509_FILE_IO_ERROR                    -0x2B00  /**< Read/write of file failed. */
#define POLARSSL_ERR_X509_PASSWORD_REQUIRED                -0x2B80  /**< Private key password can't be empty. */
#define POLARSSL_ERR_X509_PASSWORD_MISMATCH                -0x2C00  /**< Given private key password does not allow for correct decryption. */
#define POLARSSL_ERR_X509_UNKNOWN_NAMED_CURVE              -0x2C80  /**< Elliptic curve is unsupported (only NIST curves are supported). */
/* \} name */

/**
 * \name X509 Verify codes
 * \{
 */
#define BADCERT_EXPIRED             0x01  /**< The certificate validity has expired. */
#define BADCERT_REVOKED             0x02  /**< The certificate has been revoked (is on a CRL). */
#define BADCERT_CN_MISMATCH         0x04  /**< The certificate Common Name (CN) does not match with the expected CN. */
#define BADCERT_NOT_TRUSTED         0x08  /**< The certificate is not correctly signed by the trusted CA. */
#define BADCRL_NOT_TRUSTED          0x10  /**< CRL is not correctly signed by the trusted CA. */
#define BADCRL_EXPIRED              0x20  /**< CRL is expired. */
#define BADCERT_MISSING             0x40  /**< Certificate was missing. */
#define BADCERT_SKIP_VERIFY         0x80  /**< Certificate verification was skipped. */
#define BADCERT_OTHER             0x0100  /**< Other reason (can be used by verify callback) */
/* \} name */
/* \} addtogroup x509_module */

/*
 * X.509 v3 Key Usage Extension flags
 */
#define KU_DIGITAL_SIGNATURE            (0x80)  /* bit 0 */
#define KU_NON_REPUDIATION              (0x40)  /* bit 1 */
#define KU_KEY_ENCIPHERMENT             (0x20)  /* bit 2 */
#define KU_DATA_ENCIPHERMENT            (0x10)  /* bit 3 */
#define KU_KEY_AGREEMENT                (0x08)  /* bit 4 */
#define KU_KEY_CERT_SIGN                (0x04)  /* bit 5 */
#define KU_CRL_SIGN                     (0x02)  /* bit 6 */

/*
 * Netscape certificate types
 * (http://www.mozilla.org/projects/security/pki/nss/tech-notes/tn3.html)
 */

#define NS_CERT_TYPE_SSL_CLIENT         (0x80)  /* bit 0 */
#define NS_CERT_TYPE_SSL_SERVER         (0x40)  /* bit 1 */
#define NS_CERT_TYPE_EMAIL              (0x20)  /* bit 2 */
#define NS_CERT_TYPE_OBJECT_SIGNING     (0x10)  /* bit 3 */
#define NS_CERT_TYPE_RESERVED           (0x08)  /* bit 4 */
#define NS_CERT_TYPE_SSL_CA             (0x04)  /* bit 5 */
#define NS_CERT_TYPE_EMAIL_CA           (0x02)  /* bit 6 */
#define NS_CERT_TYPE_OBJECT_SIGNING_CA  (0x01)  /* bit 7 */

/*
 * X.509 extension types
 */
#define EXT_AUTHORITY_KEY_IDENTIFIER    (1 << 0)
#define EXT_SUBJECT_KEY_IDENTIFIER      (1 << 1)
#define EXT_KEY_USAGE                   (1 << 2)
#define EXT_CERTIFICATE_POLICIES        (1 << 3)
#define EXT_POLICY_MAPPINGS             (1 << 4)
#define EXT_SUBJECT_ALT_NAME            (1 << 5)
#define EXT_ISSUER_ALT_NAME             (1 << 6)
#define EXT_SUBJECT_DIRECTORY_ATTRS     (1 << 7)
#define EXT_BASIC_CONSTRAINTS           (1 << 8)
#define EXT_NAME_CONSTRAINTS            (1 << 9)
#define EXT_POLICY_CONSTRAINTS          (1 << 10)
#define EXT_EXTENDED_KEY_USAGE          (1 << 11)
#define EXT_CRL_DISTRIBUTION_POINTS     (1 << 12)
#define EXT_INIHIBIT_ANYPOLICY          (1 << 13)
#define EXT_FRESHEST_CRL                (1 << 14)

#define EXT_NS_CERT_TYPE                (1 << 16)

/*
 * Storage format identifiers
 * Recognized formats: PEM and DER
 */
#define X509_FORMAT_DER                 1
#define X509_FORMAT_PEM                 2

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup x509_module
 * \{ */

/**
 * \name Structures for parsing X.509 certificates, CRLs and CSRs
 * \{
 */

/**
 * Type-length-value structure that allows for ASN1 using DER.
 */
typedef asn1_buf x509_buf;

/**
 * Container for ASN1 bit strings.
 */
typedef asn1_bitstring x509_bitstring;

/**
 * Container for ASN1 named information objects.
 * It allows for Relative Distinguished Names (e.g. cn=polarssl,ou=code,etc.).
 */
typedef asn1_named_data x509_name;

/**
 * Container for a sequence of ASN.1 items
 */
typedef asn1_sequence x509_sequence;

/** Container for date and time (precision in seconds). */
typedef struct _x509_time
{
    int year, mon, day;         /**< Date. */
    int hour, min, sec;         /**< Time. */
}
x509_time;

/**
 * Container for an X.509 certificate. The certificate may be chained.
 */
typedef struct _x509_cert
{
    x509_buf raw;               /**< The raw certificate data (DER). */
    x509_buf tbs;               /**< The raw certificate body (DER). The part that is To Be Signed. */

    int version;                /**< The X.509 version. (0=v1, 1=v2, 2=v3) */
    x509_buf serial;            /**< Unique id for certificate issued by a specific CA. */
    x509_buf sig_oid1;          /**< Signature algorithm, e.g. sha1RSA */

    x509_buf issuer_raw;        /**< The raw issuer data (DER). Used for quick comparison. */
    x509_buf subject_raw;       /**< The raw subject data (DER). Used for quick comparison. */

    x509_name issuer;           /**< The parsed issuer data (named information object). */
    x509_name subject;          /**< The parsed subject data (named information object). */

    x509_time valid_from;       /**< Start time of certificate validity. */
    x509_time valid_to;         /**< End time of certificate validity. */

    pk_context pk;              /**< Container for the public key context. */

    x509_buf issuer_id;         /**< Optional X.509 v2/v3 issuer unique identifier. */
    x509_buf subject_id;        /**< Optional X.509 v2/v3 subject unique identifier. */
    x509_buf v3_ext;            /**< Optional X.509 v3 extensions. Only Basic Contraints are supported at this time. */
    x509_sequence subject_alt_names;    /**< Optional list of Subject Alternative Names (Only dNSName supported). */

    int ext_types;              /**< Bit string containing detected and parsed extensions */
    int ca_istrue;              /**< Optional Basic Constraint extension value: 1 if this certificate belongs to a CA, 0 otherwise. */
    int max_pathlen;            /**< Optional Basic Constraint extension value: The maximum path length to the root certificate. Path length is 1 higher than RFC 5280 'meaning', so 1+ */

    unsigned char key_usage;    /**< Optional key usage extension value: See the values below */

    x509_sequence ext_key_usage; /**< Optional list of extended key usage OIDs. */

    unsigned char ns_cert_type; /**< Optional Netscape certificate type extension value: See the values below */

    x509_buf sig_oid2;          /**< Signature algorithm. Must match sig_oid1. */
    x509_buf sig;               /**< Signature: hash of the tbs part signed with the private key. */
    md_type_t sig_md;           /**< Internal representation of the MD algorithm of the signature algorithm, e.g. POLARSSL_MD_SHA256 */
    pk_type_t sig_pk            /**< Internal representation of the Public Key algorithm of the signature algorithm, e.g. POLARSSL_PK_RSA */;

    struct _x509_cert *next;    /**< Next certificate in the CA-chain. */
}
x509_cert;

/**
 * Certificate revocation list entry.
 * Contains the CA-specific serial numbers and revocation dates.
 */
typedef struct _x509_crl_entry
{
    x509_buf raw;

    x509_buf serial;

    x509_time revocation_date;

    x509_buf entry_ext;

    struct _x509_crl_entry *next;
}
x509_crl_entry;

/**
 * Certificate revocation list structure.
 * Every CRL may have multiple entries.
 */
typedef struct _x509_crl
{
    x509_buf raw;           /**< The raw certificate data (DER). */
    x509_buf tbs;           /**< The raw certificate body (DER). The part that is To Be Signed. */

    int version;
    x509_buf sig_oid1;

    x509_buf issuer_raw;    /**< The raw issuer data (DER). */

    x509_name issuer;       /**< The parsed issuer data (named information object). */

    x509_time this_update;
    x509_time next_update;

    x509_crl_entry entry;   /**< The CRL entries containing the certificate revocation times for this CA. */

    x509_buf crl_ext;

    x509_buf sig_oid2;
    x509_buf sig;
    md_type_t sig_md;           /**< Internal representation of the MD algorithm of the signature algorithm, e.g. POLARSSL_MD_SHA256 */
    pk_type_t sig_pk            /**< Internal representation of the Public Key algorithm of the signature algorithm, e.g. POLARSSL_PK_RSA */;

    struct _x509_crl *next;
}
x509_crl;

/**
 * Certificate Signing Request (CSR) structure.
 */
typedef struct _x509_csr
{
    x509_buf raw;           /**< The raw CSR data (DER). */
    x509_buf cri;           /**< The raw CertificateRequestInfo body (DER). */

    int version;

    x509_buf  subject_raw;  /**< The raw subject data (DER). */
    x509_name subject;      /**< The parsed subject data (named information object). */

    pk_context pk;          /**< Container for the public key context. */

    x509_buf sig_oid;
    x509_buf sig;
    md_type_t sig_md;       /**< Internal representation of the MD algorithm of the signature algorithm, e.g. POLARSSL_MD_SHA256 */
    pk_type_t sig_pk        /**< Internal representation of the Public Key algorithm of the signature algorithm, e.g. POLARSSL_PK_RSA */;
}
x509_csr;
/** \} name Structures for parsing X.509 certificates, CRLs and CSRs */
/** \} addtogroup x509_module */

/**
 * \name Functions to read in DHM parameters, a certificate, CRL or private RSA key
 * \{
 */

/** \ingroup x509_module */
/**
 * \brief          Parse a single DER formatted certificate and add it
 *                 to the chained list.
 *
 * \param chain    points to the start of the chain
 * \param buf      buffer holding the certificate DER data
 * \param buflen   size of the buffer
 *
 * \return         0 if successful, or a specific X509 or PEM error code
 */
int x509parse_crt_der( x509_cert *chain, const unsigned char *buf, size_t buflen );

/**
 * \brief          Parse one or more certificates and add them
 *                 to the chained list. Parses permissively. If some
 *                 certificates can be parsed, the result is the number
 *                 of failed certificates it encountered. If none complete
 *                 correctly, the first error is returned.
 *
 * \param chain    points to the start of the chain
 * \param buf      buffer holding the certificate data
 * \param buflen   size of the buffer
 *
 * \return         0 if all certificates parsed successfully, a positive number
 *                 if partly successful or a specific X509 or PEM error code
 */
int x509parse_crt( x509_cert *chain, const unsigned char *buf, size_t buflen );

#if defined(POLARSSL_FS_IO)
/** \ingroup x509_module */
/**
 * \brief          Load one or more certificates and add them
 *                 to the chained list. Parses permissively. If some
 *                 certificates can be parsed, the result is the number
 *                 of failed certificates it encountered. If none complete
 *                 correctly, the first error is returned.
 *
 * \param chain    points to the start of the chain
 * \param path     filename to read the certificates from
 *
 * \return         0 if all certificates parsed successfully, a positive number
 *                 if partly successful or a specific X509 or PEM error code
 */
int x509parse_crtfile( x509_cert *chain, const char *path );

/** \ingroup x509_module */
/**
 * \brief          Load one or more certificate files from a path and add them
 *                 to the chained list. Parses permissively. If some
 *                 certificates can be parsed, the result is the number
 *                 of failed certificates it encountered. If none complete
 *                 correctly, the first error is returned.
 *
 * \param chain    points to the start of the chain
 * \param path     directory / folder to read the certificate files from
 *
 * \return         0 if all certificates parsed successfully, a positive number
 *                 if partly successful or a specific X509 or PEM error code
 */
int x509parse_crtpath( x509_cert *chain, const char *path );
#endif /* POLARSSL_FS_IO */

/** \ingroup x509_module */
/**
 * \brief          Parse one or more CRLs and add them
 *                 to the chained list
 *
 * \param chain    points to the start of the chain
 * \param buf      buffer holding the CRL data
 * \param buflen   size of the buffer
 *
 * \return         0 if successful, or a specific X509 or PEM error code
 */
int x509parse_crl( x509_crl *chain, const unsigned char *buf, size_t buflen );

/** \ingroup x509_module */
/**
 * \brief          Load a Certificate Signing Request (CSR)
 *
 * \param csr      CSR context to fill
 * \param buf      buffer holding the CRL data
 * \param buflen   size of the buffer
 *
 * \return         0 if successful, or a specific X509 or PEM error code
 */
int x509parse_csr( x509_csr *csr, const unsigned char *buf, size_t buflen );

#if defined(POLARSSL_FS_IO)
/** \ingroup x509_module */
/**
 * \brief          Load one or more CRLs and add them
 *                 to the chained list
 *
 * \param chain    points to the start of the chain
 * \param path     filename to read the CRLs from
 *
 * \return         0 if successful, or a specific X509 or PEM error code
 */
int x509parse_crlfile( x509_crl *chain, const char *path );

/** \ingroup x509_module */
/**
 * \brief          Load a Certificate Signing Request (CSR)
 *
 * \param csr      CSR context to fill
 * \param path     filename to read the CSR from
 *
 * \return         0 if successful, or a specific X509 or PEM error code
 */
int x509parse_csrfile( x509_csr *csr, const char *path );
#endif /* POLARSSL_FS_IO */

#if defined(POLARSSL_RSA_C)
/** \ingroup x509_module */
/**
 * \brief          Parse a private RSA key
 *
 * \param rsa      RSA context to be initialized
 * \param key      input buffer
 * \param keylen   size of the buffer
 * \param pwd      password for decryption (optional)
 * \param pwdlen   size of the password
 *
 * \return         0 if successful, or a specific X509 or PEM error code
 */
int x509parse_key_rsa( rsa_context *rsa,
                       const unsigned char *key, size_t keylen,
                       const unsigned char *pwd, size_t pwdlen );

#if defined(POLARSSL_FS_IO)
/** \ingroup x509_module */
/**
 * \brief          Load and parse a private RSA key
 *
 * \param rsa      RSA context to be initialized
 * \param path     filename to read the private key from
 * \param password password to decrypt the file (can be NULL)
 *
 * \return         0 if successful, or a specific X509 or PEM error code
 */
int x509parse_keyfile_rsa( rsa_context *rsa, const char *path,
                           const char *password );
#endif /* POLARSSL_FS_IO */

/** \ingroup x509_module */
/**
 * \brief          Parse a public RSA key
 *
 * \param rsa      RSA context to be initialized
 * \param key      input buffer
 * \param keylen   size of the buffer
 *
 * \return         0 if successful, or a specific X509 or PEM error code
 */
int x509parse_public_key_rsa( rsa_context *rsa,
                       const unsigned char *key, size_t keylen );

#if defined(POLARSSL_FS_IO)
/** \ingroup x509_module */
/**
 * \brief          Load and parse a public RSA key
 *
 * \param rsa      RSA context to be initialized
 * \param path     filename to read the private key from
 *
 * \return         0 if successful, or a specific X509 or PEM error code
 */
int x509parse_public_keyfile_rsa( rsa_context *rsa, const char *path );
#endif /* POLARSSL_FS_IO */
#endif /* POLARSSL_RSA_C */

/** \ingroup x509_module */
/**
 * \brief          Parse a private key
 *
 * \param ctx      key to be initialized
 * \param key      input buffer
 * \param keylen   size of the buffer
 * \param pwd      password for decryption (optional)
 * \param pwdlen   size of the password
 *
 * \return         0 if successful, or a specific X509 or PEM error code
 */
int x509parse_key( pk_context *ctx,
                   const unsigned char *key, size_t keylen,
                   const unsigned char *pwd, size_t pwdlen );

#if defined(POLARSSL_FS_IO)
/** \ingroup x509_module */
/**
 * \brief          Load and parse a private key
 *
 * \param ctx      key to be initialized
 * \param path     filename to read the private key from
 * \param password password to decrypt the file (can be NULL)
 *
 * \return         0 if successful, or a specific X509 or PEM error code
 */
int x509parse_keyfile( pk_context *ctx,
                       const char *path, const char *password );
#endif /* POLARSSL_FS_IO */

/** \ingroup x509_module */
/**
 * \brief          Parse a public key
 *
 * \param ctx      key to be initialized
 * \param key      input buffer
 * \param keylen   size of the buffer
 *
 * \return         0 if successful, or a specific X509 or PEM error code
 */
int x509parse_public_key( pk_context *ctx,
                          const unsigned char *key, size_t keylen );

#if defined(POLARSSL_FS_IO)
/** \ingroup x509_module */
/**
 * \brief          Load and parse a public key
 *
 * \param ctx      key to be initialized
 * \param path     filename to read the private key from
 *
 * \return         0 if successful, or a specific X509 or PEM error code
 */
int x509parse_public_keyfile( pk_context *ctx, const char *path );
#endif /* POLARSSL_FS_IO */

/** \ingroup x509_module */
/**
 * \brief          Parse DHM parameters
 *
 * \param dhm      DHM context to be initialized
 * \param dhmin    input buffer
 * \param dhminlen size of the buffer
 *
 * \return         0 if successful, or a specific X509 or PEM error code
 */
int x509parse_dhm( dhm_context *dhm, const unsigned char *dhmin, size_t dhminlen );

#if defined(POLARSSL_FS_IO)
/** \ingroup x509_module */
/**
 * \brief          Load and parse DHM parameters
 *
 * \param dhm      DHM context to be initialized
 * \param path     filename to read the DHM Parameters from
 *
 * \return         0 if successful, or a specific X509 or PEM error code
 */
int x509parse_dhmfile( dhm_context *dhm, const char *path );
#endif /* POLARSSL_FS_IO */

/** \} name Functions to read in DHM parameters, a certificate, CRL or private RSA key */

/**
 * \brief          Store the certificate DN in printable form into buf;
 *                 no more than size characters will be written.
 *
 * \param buf      Buffer to write to
 * \param size     Maximum size of buffer
 * \param dn       The X509 name to represent
 *
 * \return         The amount of data written to the buffer, or -1 in
 *                 case of an error.
 */
int x509parse_dn_gets( char *buf, size_t size, const x509_name *dn );

/**
 * \brief          Store the certificate serial in printable form into buf;
 *                 no more than size characters will be written.
 *
 * \param buf      Buffer to write to
 * \param size     Maximum size of buffer
 * \param serial   The X509 serial to represent
 *
 * \return         The amount of data written to the buffer, or -1 in
 *                 case of an error.
 */
int x509parse_serial_gets( char *buf, size_t size, const x509_buf *serial );

/**
 * \brief          Returns an informational string about the
 *                 certificate.
 *
 * \param buf      Buffer to write to
 * \param size     Maximum size of buffer
 * \param prefix   A line prefix
 * \param crt      The X509 certificate to represent
 *
 * \return         The amount of data written to the buffer, or -1 in
 *                 case of an error.
 */
int x509parse_cert_info( char *buf, size_t size, const char *prefix,
                         const x509_cert *crt );

/**
 * \brief          Returns an informational string about the
 *                 CRL.
 *
 * \param buf      Buffer to write to
 * \param size     Maximum size of buffer
 * \param prefix   A line prefix
 * \param crl      The X509 CRL to represent
 *
 * \return         The amount of data written to the buffer, or -1 in
 *                 case of an error.
 */
int x509parse_crl_info( char *buf, size_t size, const char *prefix,
                        const x509_crl *crl );

/**
 * \brief          Returns an informational string about the
 *                 CSR.
 *
 * \param buf      Buffer to write to
 * \param size     Maximum size of buffer
 * \param prefix   A line prefix
 * \param csr      The X509 CSR to represent
 *
 * \return         The amount of data written to the buffer, or -1 in
 *                 case of an error.
 */
int x509parse_csr_info( char *buf, size_t size, const char *prefix,
                        const x509_csr *csr );

/**
 * \brief          Give an known OID, return its descriptive string.
 *
 * \param oid      buffer containing the oid
 *
 * \return         Return a string if the OID is known,
 *                 or NULL otherwise.
 */
const char *x509_oid_get_description( x509_buf *oid );

/**
 * \brief          Give an OID, return a string version of its OID number.
 *                 (Deprecated. Use oid_get_numeric_string() instead)
 *
 * \param buf      Buffer to write to
 * \param size     Maximum size of buffer
 * \param oid      Buffer containing the OID
 *
 * \return         The amount of data written to the buffer, or -1 in
 *                 case of an error.
 */
int x509_oid_get_numeric_string( char *buf, size_t size, x509_buf *oid );

/**
 * \brief          Check a given x509_time against the system time and check
 *                 if it is valid.
 *
 * \param time     x509_time to check
 *
 * \return         Return 0 if the x509_time is still valid,
 *                 or 1 otherwise.
 */
int x509parse_time_expired( const x509_time *time );

/**
 * \name Functions to verify a certificate
 * \{
 */
/** \ingroup x509_module */
/**
 * \brief          Verify the certificate signature
 *
 *                 The verify callback is a user-supplied callback that
 *                 can clear / modify / add flags for a certificate. If set,
 *                 the verification callback is called for each
 *                 certificate in the chain (from the trust-ca down to the
 *                 presented crt). The parameters for the callback are:
 *                 (void *parameter, x509_cert *crt, int certificate_depth,
 *                 int *flags). With the flags representing current flags for
 *                 that specific certificate and the certificate depth from
 *                 the bottom (Peer cert depth = 0).
 *
 *                 All flags left after returning from the callback
 *                 are also returned to the application. The function should
 *                 return 0 for anything but a fatal error.
 *
 * \param crt      a certificate to be verified
 * \param trust_ca the trusted CA chain
 * \param ca_crl   the CRL chain for trusted CA's
 * \param cn       expected Common Name (can be set to
 *                 NULL if the CN must not be verified)
 * \param flags    result of the verification
 * \param f_vrfy   verification function
 * \param p_vrfy   verification parameter
 *
 * \return         0 if successful or POLARSSL_ERR_X509_SIG_VERIFY_FAILED,
 *                 in which case *flags will have one or more of
 *                 the following values set:
 *                      BADCERT_EXPIRED --
 *                      BADCERT_REVOKED --
 *                      BADCERT_CN_MISMATCH --
 *                      BADCERT_NOT_TRUSTED
 *                 or another error in case of a fatal error encountered
 *                 during the verification process.
 */
int x509parse_verify( x509_cert *crt,
                      x509_cert *trust_ca,
                      x509_crl *ca_crl,
                      const char *cn, int *flags,
                      int (*f_vrfy)(void *, x509_cert *, int, int *),
                      void *p_vrfy );

/**
 * \brief          Verify the certificate signature
 *
 * \param crt      a certificate to be verified
 * \param crl      the CRL to verify against
 *
 * \return         1 if the certificate is revoked, 0 otherwise
 *
 */
int x509parse_revoked( const x509_cert *crt, const x509_crl *crl );

/** \} name Functions to verify a certificate */



/**
 * \name Functions to clear a certificate, CRL or private RSA key 
 * \{
 */
/** \ingroup x509_module */
/**
 * \brief          Unallocate all certificate data
 *
 * \param crt      Certificate chain to free
 */
void x509_free( x509_cert *crt );

/** \ingroup x509_module */
/**
 * \brief          Unallocate all CRL data
 *
 * \param crl      CRL chain to free
 */
void x509_crl_free( x509_crl *crl );

/**
 * \brief          Unallocate all CSR data
 *
 * \param csr      CSR to free
 */
void x509_csr_free( x509_csr *csr );

/** \} name Functions to clear a certificate, CRL or private RSA key */


/**
 * \brief          Checkup routine
 *
 * \return         0 if successful, or 1 if the test failed
 */
int x509_self_test( int verbose );

#ifdef __cplusplus
}
#endif

#endif /* POLARSSL_X509_PARSE_C || POLARSSL_X509_WRITE_C */
#endif /* x509.h */
