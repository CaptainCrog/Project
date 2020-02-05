// Code for the WiFi/IoT Core implementation can be found at https://github.com/Nilhcem/esp32-cloud-iot-core-k8s
// This file contains your configuration used to connect to Cloud IoT Core

// Wifi network details.
const char *ssid = "Insert WiFi Name";
const char *password = "Insert WiFi Password";

// Cloud IoT details.
const char *project_id = "pen14541450-uolproject-266014";
const char *region = "europe-west1";
const char *registry_id = "iotcore-registry";
const char *device_id = "iot-car";

// Configuration for NTP
const char* ntp_primary = "pool.ntp.org";
const char* ntp_secondary = "time.nist.gov";

// To get the private key run (where private-key.pem is the ec private key
// used to create the certificate uploaded to google cloud iot):
// openssl ec -in <private-key.pem> -noout -text
// and copy priv: part.
// The key length should be exactly the same as the key length bellow (32 pairs
// of hex digits). If it's bigger and it starts with "00:" delete the "00:". If
// it's smaller add "00:" to the start. If it's too big or too small something
// is probably wrong with your key.
const char* private_key_str =
"a9:a3:be:fc:b1:b5:55:fc:89:72:66:2b:18:62:13:"
"5e:45:fe:e8:1b:7e:bd:86:57:28:33:06:43:c8:1f:"
"93:4f";

// Time (seconds) to expire token += 20 minutes for drift
const int jwt_exp_secs = 3600; // Maximum 24H (3600*24)

// In case we ever need extra topics
const int ex_num_topics = 0;
const char* ex_topics[ex_num_topics];
//const int ex_num_topics = 1;
//const char* ex_topics[ex_num_topics] = {
//  "/devices/my-device/tbd/#"
//};


// Use the root certificate to verify tls connection rather than
// using a fingerprint.
//
// To do this, install the Esp8266 Data Upload tool and upload the cert from
// the data folder by selecting Tools > ESP8266 Data Upload.
//
// Alternatively, specify a binary root cert static, e.g.:
//
static const uint8_t ca_crt[] PROGMEM = {
 0x30, 0x82, 0x05, 0x5a, 0x30, 0x82, 0x03, 0x42, 0xa0, 0x03, 0x02, 0x01,
 0x02, 0x02, 0x10, 0x6e, 0x47, 0xa9, 0xc5, 0x4b, 0x47, 0x0c, 0x0d, 0xec,
 0x33, 0xd0, 0x89, 0xb9, 0x1c, 0xf4, 0xe1, 0x30, 0x0d, 0x06, 0x09, 0x2a,
 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x0c, 0x05, 0x00, 0x30, 0x47,
 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x55,
 0x53, 0x31, 0x22, 0x30, 0x20, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x13, 0x19,
 0x47, 0x6f, 0x6f, 0x67, 0x6c, 0x65, 0x20, 0x54, 0x72, 0x75, 0x73, 0x74,
 0x20, 0x53, 0x65, 0x72, 0x76, 0x69, 0x63, 0x65, 0x73, 0x20, 0x4c, 0x4c,
 0x43, 0x31, 0x14, 0x30, 0x12, 0x06, 0x03, 0x55, 0x04, 0x03, 0x13, 0x0b,
 0x47, 0x54, 0x53, 0x20, 0x52, 0x6f, 0x6f, 0x74, 0x20, 0x52, 0x31, 0x30,
 0x1e, 0x17, 0x0d, 0x31, 0x36, 0x30, 0x36, 0x32, 0x32, 0x30, 0x30, 0x30,
 0x30, 0x30, 0x30, 0x5a, 0x17, 0x0d, 0x33, 0x36, 0x30, 0x36, 0x32, 0x32,
 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x5a, 0x30, 0x47, 0x31, 0x0b, 0x30,
 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x55, 0x53, 0x31, 0x22,
 0x30, 0x20, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x13, 0x19, 0x47, 0x6f, 0x6f,
 0x67, 0x6c, 0x65, 0x20, 0x54, 0x72, 0x75, 0x73, 0x74, 0x20, 0x53, 0x65,
 0x72, 0x76, 0x69, 0x63, 0x65, 0x73, 0x20, 0x4c, 0x4c, 0x43, 0x31, 0x14,
 0x30, 0x12, 0x06, 0x03, 0x55, 0x04, 0x03, 0x13, 0x0b, 0x47, 0x54, 0x53,
 0x20, 0x52, 0x6f, 0x6f, 0x74, 0x20, 0x52, 0x31, 0x30, 0x82, 0x02, 0x22,
 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01,
 0x01, 0x05, 0x00, 0x03, 0x82, 0x02, 0x0f, 0x00, 0x30, 0x82, 0x02, 0x0a,
 0x02, 0x82, 0x02, 0x01, 0x00, 0xb6, 0x11, 0x02, 0x8b, 0x1e, 0xe3, 0xa1,
 0x77, 0x9b, 0x3b, 0xdc, 0xbf, 0x94, 0x3e, 0xb7, 0x95, 0xa7, 0x40, 0x3c,
 0xa1, 0xfd, 0x82, 0xf9, 0x7d, 0x32, 0x06, 0x82, 0x71, 0xf6, 0xf6, 0x8c,
 0x7f, 0xfb, 0xe8, 0xdb, 0xbc, 0x6a, 0x2e, 0x97, 0x97, 0xa3, 0x8c, 0x4b,
 0xf9, 0x2b, 0xf6, 0xb1, 0xf9, 0xce, 0x84, 0x1d, 0xb1, 0xf9, 0xc5, 0x97,
 0xde, 0xef, 0xb9, 0xf2, 0xa3, 0xe9, 0xbc, 0x12, 0x89, 0x5e, 0xa7, 0xaa,
 0x52, 0xab, 0xf8, 0x23, 0x27, 0xcb, 0xa4, 0xb1, 0x9c, 0x63, 0xdb, 0xd7,
 0x99, 0x7e, 0xf0, 0x0a, 0x5e, 0xeb, 0x68, 0xa6, 0xf4, 0xc6, 0x5a, 0x47,
 0x0d, 0x4d, 0x10, 0x33, 0xe3, 0x4e, 0xb1, 0x13, 0xa3, 0xc8, 0x18, 0x6c,
 0x4b, 0xec, 0xfc, 0x09, 0x90, 0xdf, 0x9d, 0x64, 0x29, 0x25, 0x23, 0x07,
 0xa1, 0xb4, 0xd2, 0x3d, 0x2e, 0x60, 0xe0, 0xcf, 0xd2, 0x09, 0x87, 0xbb,
 0xcd, 0x48, 0xf0, 0x4d, 0xc2, 0xc2, 0x7a, 0x88, 0x8a, 0xbb, 0xba, 0xcf,
 0x59, 0x19, 0xd6, 0xaf, 0x8f, 0xb0, 0x07, 0xb0, 0x9e, 0x31, 0xf1, 0x82,
 0xc1, 0xc0, 0xdf, 0x2e, 0xa6, 0x6d, 0x6c, 0x19, 0x0e, 0xb5, 0xd8, 0x7e,
 0x26, 0x1a, 0x45, 0x03, 0x3d, 0xb0, 0x79, 0xa4, 0x94, 0x28, 0xad, 0x0f,
 0x7f, 0x26, 0xe5, 0xa8, 0x08, 0xfe, 0x96, 0xe8, 0x3c, 0x68, 0x94, 0x53,
 0xee, 0x83, 0x3a, 0x88, 0x2b, 0x15, 0x96, 0x09, 0xb2, 0xe0, 0x7a, 0x8c,
 0x2e, 0x75, 0xd6, 0x9c, 0xeb, 0xa7, 0x56, 0x64, 0x8f, 0x96, 0x4f, 0x68,
 0xae, 0x3d, 0x97, 0xc2, 0x84, 0x8f, 0xc0, 0xbc, 0x40, 0xc0, 0x0b, 0x5c,
 0xbd, 0xf6, 0x87, 0xb3, 0x35, 0x6c, 0xac, 0x18, 0x50, 0x7f, 0x84, 0xe0,
 0x4c, 0xcd, 0x92, 0xd3, 0x20, 0xe9, 0x33, 0xbc, 0x52, 0x99, 0xaf, 0x32,
 0xb5, 0x29, 0xb3, 0x25, 0x2a, 0xb4, 0x48, 0xf9, 0x72, 0xe1, 0xca, 0x64,
 0xf7, 0xe6, 0x82, 0x10, 0x8d, 0xe8, 0x9d, 0xc2, 0x8a, 0x88, 0xfa, 0x38,
 0x66, 0x8a, 0xfc, 0x63, 0xf9, 0x01, 0xf9, 0x78, 0xfd, 0x7b, 0x5c, 0x77,
 0xfa, 0x76, 0x87, 0xfa, 0xec, 0xdf, 0xb1, 0x0e, 0x79, 0x95, 0x57, 0xb4,
 0xbd, 0x26, 0xef, 0xd6, 0x01, 0xd1, 0xeb, 0x16, 0x0a, 0xbb, 0x8e, 0x0b,
 0xb5, 0xc5, 0xc5, 0x8a, 0x55, 0xab, 0xd3, 0xac, 0xea, 0x91, 0x4b, 0x29,
 0xcc, 0x19, 0xa4, 0x32, 0x25, 0x4e, 0x2a, 0xf1, 0x65, 0x44, 0xd0, 0x02,
 0xce, 0xaa, 0xce, 0x49, 0xb4, 0xea, 0x9f, 0x7c, 0x83, 0xb0, 0x40, 0x7b,
 0xe7, 0x43, 0xab, 0xa7, 0x6c, 0xa3, 0x8f, 0x7d, 0x89, 0x81, 0xfa, 0x4c,
 0xa5, 0xff, 0xd5, 0x8e, 0xc3, 0xce, 0x4b, 0xe0, 0xb5, 0xd8, 0xb3, 0x8e,
 0x45, 0xcf, 0x76, 0xc0, 0xed, 0x40, 0x2b, 0xfd, 0x53, 0x0f, 0xb0, 0xa7,
 0xd5, 0x3b, 0x0d, 0xb1, 0x8a, 0xa2, 0x03, 0xde, 0x31, 0xad, 0xcc, 0x77,
 0xea, 0x6f, 0x7b, 0x3e, 0xd6, 0xdf, 0x91, 0x22, 0x12, 0xe6, 0xbe, 0xfa,
 0xd8, 0x32, 0xfc, 0x10, 0x63, 0x14, 0x51, 0x72, 0xde, 0x5d, 0xd6, 0x16,
 0x93, 0xbd, 0x29, 0x68, 0x33, 0xef, 0x3a, 0x66, 0xec, 0x07, 0x8a, 0x26,
 0xdf, 0x13, 0xd7, 0x57, 0x65, 0x78, 0x27, 0xde, 0x5e, 0x49, 0x14, 0x00,
 0xa2, 0x00, 0x7f, 0x9a, 0xa8, 0x21, 0xb6, 0xa9, 0xb1, 0x95, 0xb0, 0xa5,
 0xb9, 0x0d, 0x16, 0x11, 0xda, 0xc7, 0x6c, 0x48, 0x3c, 0x40, 0xe0, 0x7e,
 0x0d, 0x5a, 0xcd, 0x56, 0x3c, 0xd1, 0x97, 0x05, 0xb9, 0xcb, 0x4b, 0xed,
 0x39, 0x4b, 0x9c, 0xc4, 0x3f, 0xd2, 0x55, 0x13, 0x6e, 0x24, 0xb0, 0xd6,
 0x71, 0xfa, 0xf4, 0xc1, 0xba, 0xcc, 0xed, 0x1b, 0xf5, 0xfe, 0x81, 0x41,
 0xd8, 0x00, 0x98, 0x3d, 0x3a, 0xc8, 0xae, 0x7a, 0x98, 0x37, 0x18, 0x05,
 0x95, 0x02, 0x03, 0x01, 0x00, 0x01, 0xa3, 0x42, 0x30, 0x40, 0x30, 0x0e,
 0x06, 0x03, 0x55, 0x1d, 0x0f, 0x01, 0x01, 0xff, 0x04, 0x04, 0x03, 0x02,
 0x01, 0x06, 0x30, 0x0f, 0x06, 0x03, 0x55, 0x1d, 0x13, 0x01, 0x01, 0xff,
 0x04, 0x05, 0x30, 0x03, 0x01, 0x01, 0xff, 0x30, 0x1d, 0x06, 0x03, 0x55,
 0x1d, 0x0e, 0x04, 0x16, 0x04, 0x14, 0xe4, 0xaf, 0x2b, 0x26, 0x71, 0x1a,
 0x2b, 0x48, 0x27, 0x85, 0x2f, 0x52, 0x66, 0x2c, 0xef, 0xf0, 0x89, 0x13,
 0x71, 0x3e, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d,
 0x01, 0x01, 0x0c, 0x05, 0x00, 0x03, 0x82, 0x02, 0x01, 0x00, 0x38, 0x96,
 0x0a, 0xee, 0x3d, 0xb4, 0x96, 0x1e, 0x5f, 0xef, 0x9d, 0x9c, 0x0b, 0x33,
 0x9f, 0x2b, 0xe0, 0xca, 0xfd, 0xd2, 0x8e, 0x0a, 0x1f, 0x41, 0x74, 0xa5,
 0x7c, 0xaa, 0x84, 0xd4, 0xe5, 0xf2, 0x1e, 0xe6, 0x37, 0x52, 0x32, 0x9c,
 0x0b, 0xd1, 0x61, 0x1d, 0xbf, 0x28, 0xc1, 0xb6, 0x44, 0x29, 0x35, 0x75,
 0x77, 0x98, 0xb2, 0x7c, 0xd9, 0xbd, 0x74, 0xac, 0x8a, 0x68, 0xe3, 0xa9,
 0x31, 0x09, 0x29, 0x01, 0x60, 0x73, 0xe3, 0x47, 0x7c, 0x53, 0xa8, 0x90,
 0x4a, 0x27, 0xef, 0x4b, 0xd7, 0x9f, 0x93, 0xe7, 0x82, 0x36, 0xce, 0x9a,
 0x68, 0x0c, 0x82, 0xe7, 0xcf, 0xd4, 0x10, 0x16, 0x6f, 0x5f, 0x0e, 0x99,
 0x5c, 0xf6, 0x1f, 0x71, 0x7d, 0xef, 0xef, 0x7b, 0x2f, 0x7e, 0xea, 0x36,
 0xd6, 0x97, 0x70, 0x0b, 0x15, 0xee, 0xd7, 0x5c, 0x56, 0x6a, 0x33, 0xa5,
 0xe3, 0x49, 0x38, 0x0c, 0xb8, 0x7d, 0xfb, 0x8d, 0x85, 0xa4, 0xb1, 0x59,
 0x5e, 0xf4, 0x6a, 0xe1, 0xdd, 0xa1, 0xf6, 0x64, 0x44, 0xae, 0xe6, 0x51,
 0x83, 0x21, 0x66, 0xc6, 0x11, 0x3e, 0xf3, 0xce, 0x47, 0xee, 0x9c, 0x28,
 0x1f, 0x25, 0xda, 0xff, 0xac, 0x66, 0x95, 0xdd, 0x35, 0x0f, 0x5c, 0xef,
 0x20, 0x2c, 0x62, 0xfd, 0x91, 0xba, 0xa9, 0xcc, 0xfc, 0x5a, 0x9c, 0x93,
 0x81, 0x83, 0x29, 0x97, 0x4a, 0x7c, 0x5a, 0x72, 0xb4, 0x39, 0xd0, 0xb7,
 0x77, 0xcb, 0x79, 0xfd, 0x69, 0x3a, 0x92, 0x37, 0xed, 0x6e, 0x38, 0x65,
 0x46, 0x7e, 0xe9, 0x60, 0xbd, 0x79, 0x88, 0x97, 0x5f, 0x38, 0x12, 0xf4,
 0xee, 0xaf, 0x5b, 0x82, 0xc8, 0x86, 0xd5, 0xe1, 0x99, 0x6d, 0x8c, 0x04,
 0xf2, 0x76, 0xba, 0x49, 0xf6, 0x6e, 0xe9, 0x6d, 0x1e, 0x5f, 0xa0, 0xef,
 0x27, 0x82, 0x76, 0x40, 0xf8, 0xa6, 0xd3, 0x58, 0x5c, 0x0f, 0x2c, 0x42,
 0xda, 0x42, 0xc6, 0x7b, 0x88, 0x34, 0xc7, 0xc1, 0xd8, 0x45, 0x9b, 0xc1,
 0x3e, 0xc5, 0x61, 0x1d, 0xd9, 0x63, 0x50, 0x49, 0xf6, 0x34, 0x85, 0x6a,
 0xe0, 0x18, 0xc5, 0x6e, 0x47, 0xab, 0x41, 0x42, 0x29, 0x9b, 0xf6, 0x60,
 0x0d, 0xd2, 0x31, 0xd3, 0x63, 0x98, 0x23, 0x93, 0x5a, 0x00, 0x81, 0x48,
 0xb4, 0xef, 0xcd, 0x8a, 0xcd, 0xc9, 0xcf, 0x99, 0xee, 0xd9, 0x9e, 0xaa,
 0x36, 0xe1, 0x68, 0x4b, 0x71, 0x49, 0x14, 0x36, 0x28, 0x3a, 0x3d, 0x1d,
 0xce, 0x9a, 0x8f, 0x25, 0xe6, 0x80, 0x71, 0x61, 0x2b, 0xb5, 0x7b, 0xcc,
 0xf9, 0x25, 0x16, 0x81, 0xe1, 0x31, 0x5f, 0xa1, 0xa3, 0x7e, 0x16, 0xa4,
 0x9c, 0x16, 0x6a, 0x97, 0x18, 0xbd, 0x76, 0x72, 0xa5, 0x0b, 0x9e, 0x1d,
 0x36, 0xe6, 0x2f, 0xa1, 0x2f, 0xbe, 0x70, 0x91, 0x0f, 0xa8, 0xe6, 0xda,
 0xf8, 0xc4, 0x92, 0x40, 0x6c, 0x25, 0x7e, 0x7b, 0xb3, 0x09, 0xdc, 0xb2,
 0x17, 0xad, 0x80, 0x44, 0xf0, 0x68, 0xa5, 0x8f, 0x94, 0x75, 0xff, 0x74,
 0x5a, 0xe8, 0xa8, 0x02, 0x7c, 0x0c, 0x09, 0xe2, 0xa9, 0x4b, 0x0b, 0xa0,
 0x85, 0x0b, 0x62, 0xb9, 0xef, 0xa1, 0x31, 0x92, 0xfb, 0xef, 0xf6, 0x51,
 0x04, 0x89, 0x6c, 0xe8, 0xa9, 0x74, 0xa1, 0xbb, 0x17, 0xb3, 0xb5, 0xfd,
 0x49, 0x0f, 0x7c, 0x3c, 0xec, 0x83, 0x18, 0x20, 0x43, 0x4e, 0xd5, 0x93,
 0xba, 0xb4, 0x34, 0xb1, 0x1f, 0x16, 0x36, 0x1f, 0x0c, 0xe6, 0x64, 0x39,
 0x16, 0x4c, 0xdc, 0xe0, 0xfe, 0x1d, 0xc8, 0xa9, 0x62, 0x3d, 0x40, 0xea,
 0xca, 0xc5, 0x34, 0x02, 0xb4, 0xae, 0x89, 0x88, 0x33, 0x35, 0xdc, 0x2c,
 0x13, 0x73, 0xd8, 0x27, 0xf1, 0xd0, 0x72, 0xee, 0x75, 0x3b, 0x22, 0xde,
 0x98, 0x68, 0x66, 0x5b, 0xf1, 0xc6, 0x63, 0x47, 0x55, 0x1c, 0xba, 0xa5,
 0x08, 0x51, 0x75, 0xa6, 0x48, 0x25
};

size_t ca_crt_len = 1374;
