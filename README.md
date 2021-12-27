DCC Decoder
===========

Hacked together C code to parse DCC (EU+UK covid QR code).

Built-in base45 and CBOR decoder.

Requires zlib.

Doesn't verify signatures.

Paste QR code strings (starting HC1:) into a text file (one on each line) and specify file as argument.

Links
-----

* https://github.com/ehn-dcc-development/hcert-spec
* https://github.com/ehn-dcc-development/ehn-dcc-schema
* https://covid-status.service.nhsx.nhs.uk/pubkeys/keys.json

NHS Notes
---------

NHS allows exporting either a "Domestic" or "International" pass.

They also provide the open source "NHS Covid Pass Verifier" app, which can help provide some clues to the various accepted formats (although the GitHub is not up to date the the app store, so doesn't contain code for the new Domestic certificate).

* https://github.com/nhsx/covid-pass-verifier

### International

The International pass is a fully compliant DCC 1.3.0 certificate (object with "v", "dob", "nam", "ver" fields), 1 certificate per dose.

```
(map) 4
  (int) 1
    (string) "GB"
  (int) 4
    (int) 1643133900
  (int) 6
    (int) 1640541956
  (int) -260
    (map) 1
      (int) 1
        (map) 4
          (string) "v"
            (array) 1
              (map) 10
                (string) "ci"
                  (string) "URN:UVCI:01:GB:XXXXXXXXXXXXXXXXXXXXX#X"
                (string) "co"
                  (string) "GB"
                (string) "dn"
                  (int) 2
                (string) "dt"
                  (string) "2021-08-24"
                (string) "is"
                  (string) "NHS Digital"
                (string) "ma"
                  (string) "ORG-100030215"
                (string) "mp"
                  (string) "EU/1/20/1528"
                (string) "sd"
                  (int) 2
                (string) "tg"
                  (string) "840539006"
                (string) "vp"
                  (string) "1119349007"
          (string) "dob"
            (string) "1990-01-01"
          (string) "nam"
            (map) 4
              (string) "fn"
                (string) "POTTER"
              (string) "gn"
                (string) "HARRY"
              (string) "fnt"
                (string) "POTTER"
              (string) "gnt"
                (string) "HARRY"
          (string) "ver"
            (string) "1.3.0"
```

### Domestic

The Domestic was originally three base64 strings concatenated with '.'. The first was the key id, the second the payload, the third the signature. The decoded payload was a text string containing just an expiry date and time and full name.

It has now evolved (around about start of November) to look a lot similar to DCC, but not complient.

It differs from DCC by containing a "d" ("domestic"?) object in place of the "v", "t" or "r" object. This contains a lot less information about the actual vacination, mainly valid from and until dates. Most field are borrowed from the DCC schema, except for "pm" and "po"; it is not clear what these mean.

"ver" is 1.0.0, this might refer specifically to the Domestic standard, which is currently private to the NHS.

The aim of the Domestic certificate seems to be to assert that the user is valid according to government policy at the time, rather then for specific events such as tests and jabs. However, I will be looking to see if "pm" or "po" changes after the booster.

```
(map) 4
  (string) "1"
    (string) "GB"
  (string) "4"
    (int) 1642422480
  (string) "6"
    (int) 1639830514
  (string) "-260"
    (map) 1
      (string) "1"
        (map) 4
          (string) "d"
            (array) 1
              (map) 7
                (string) "ci"
                  (string) "URN:UVCI:01:GB:XXXXXXXXXXXXXXXXXXXXX#X"
                (string) "co"
                  (string) "GB-ENG"
                (string) "df"
                  (string) "2021-12-18T12:28:34+00:00"
                (string) "du"
                  (string) "2022-01-17T12:28:00+00:00"
                (string) "is"
                  (string) "NHS Digital"
                (string) "pm"
                  (int) 123
                (string) "po"
                  (array) 1
                    (string) "GB-ENG:3"
          (string) "dob"
            (string) "1990-01-01"
          (string) "nam"
            (map) 4
              (string) "fn"
                (string) "POTTER"
              (string) "gn"
                (string) "HARRY"
              (string) "fnt"
                (string) "POTTER"
              (string) "gnt"
                (string) "HARRY"
          (string) "ver"
            (string) "1.0.0"
```





