static const uint8_t mqttCertFingerprint[20] PROGMEM = {0x7a, 0x73, 0xe8, 0x01, 0x00, 0x4e, 0x74, 0x66, 0x78, 0x9a, 0x3c, 0x79, 0x4b, 0x4d, 0x55, 0x4c, 0x26, 0x83, 0x30, 0xf6};

// Production server api.smartfarmingunpad.com SHA-1 fingerprint
static const uint8_t sfunpadProdServerFingerprint[20] PROGMEM = {0x56, 0xec, 0x2e, 0xf1, 0xa4, 0x5e, 0x72, 0xb7, 0xbc, 0x0c, 0x9f, 0xca, 0x75, 0x9e, 0x83, 0x2c, 0x7a, 0xa1, 0xf9, 0xc7};

// Setup page
static const char* configHTML = R"(
<html>
  <head>
    <title>frianshFW Board Config</title>
    <meta name="viewport" content="width=device-width, initial-scale=1" />
    <style>
      body {
        margin: 0;
        background-color: #1abc9c;
        font-family: Arial, Helvetica, sans-serif;
        /* min-width: 100vw; */
        /* min-height: 100vh; */
      }

      h3 {
        margin: 0;
        color: #d35400;
        font-weight: 600;
        font-style: italic;
        text-shadow: 0px 0px 1px rgba(0, 0, 0, 0.15);
        text-align: center;
      }

      h5 {
        margin-bottom: 5px;
        margin-top: 16px;
        font-weight: 400;
      }

      .container {
        display: flex;
        align-items: center;
        flex-direction: column;
        /* min-width: 100vw; */
        /* min-height: 100vh; */
      }

      .main-input {
        margin: 42px 17px 17px 17px;
        background-color: #ecf0f1;
        padding: 24px 24px 10px 24px;
        border-radius: 12px;
        border-style: solid;
        border-color: #999999;
        border-width: 1px;
        min-width: 256px;
        box-shadow: 2px 4px 21px 2px rgba(0, 0, 0, 0.18);
        /* -webkit-box-shadow: 2px 4px 21px 2px rgba(0, 0, 0, 0.31);
        -moz-box-shadow: 2px 4px 21px 2px rgba(0, 0, 0, 0.31); */
      }

      .input-field-form {
        margin-top: 8px;
        display: flex;
        flex-direction: column;
        justify-content: center;
      }

      .input-text {
        padding: 8px;
        border-radius: 5px;
        border: none;
        box-shadow: 0px 0px 3px 1px rgba(0, 0, 0, 0.1);
      }

      .submit-button {
        margin-top: 24px;
        padding: 12px;
        background-color: #8e44ad;
        color: white;
        text-shadow: 1px 1px 3px rgba(0, 0, 0, 0.2);
        border-radius: 7px;
        border: none;
        box-shadow: 0px 0px 3px 1px rgba(0, 0, 0, 0.27);
      }

      .copyright-text {
        font-size: 14px;
        color: white;
        margin-top: 16px;
        text-shadow: 1px 2px 5px rgba(0, 0, 0, 0.5);
      }
    </style>
  </head>
  <body>
    <div class="container">
      <div class="main-input">
        <h3>frianshFW Board Config</h3>
        <form
          method="post"
          enctype="application/x-www-form-urlencoded"
          action="/save"
          class="input-field-form"
        >
          <h5>Cloud host:</h5>
          <input
            type="text"
            name="cloud_host"
            value="api.smartfarmingunpad.com"
            class="input-text"
          />
          <h5>Cloud https port:</h5>
          <input
            type="text"
            name="cloud_https_port"
            value="80"
            class="input-text"
          />
          <h5>Cloud mqtts port:</h5>
          <input
            type="text"
            name="cloud_mqtts_port"
            value="8883"
            class="input-text"
          />
          <h5>WiFi SSID:</h5>
          <input
            type="text"
            name="wifi_ssid"
            placeholder="your WiFi SSID"
            class="input-text"
          />
          <h5>WiFi password:</h5>
          <input
            type="password"
            name="wifi_password"
            placeholder="your WiFi password"
            class="input-text"
          />
          <h5>Device token:</h5>
          <input
            type="text"
            name="device_token"
            placeholder="your device token"
            class="input-text"
          />
          <input type="submit" value="Save" class="submit-button" />
        </form>
      </div>

      <span class="copyright-text">Copyright &copy; 2021 Fikri Rida P</span>
    </div>
  </body>
</html>
)";
