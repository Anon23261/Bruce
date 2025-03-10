#include <globals.h>
#include "core/sd_functions.h"
#include "core/main_menu.h"
#include "core/display.h"
#include "core/mykeyboard.h"
#include "bad_usb.h"
#include <vector>
#include <random>

#ifdef USB_as_HID
USBHIDKeyboard Kb;
#else
CH9329_Keyboard_ Kb;
HardwareSerial mySerial(1);
#endif

#define DEF_DELAY 100

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dis(50, 150);

void key_input(FS fs, std::vector<String> bad_scripts) {
  for (const auto& bad_script : bad_scripts) {
    if (fs.exists(bad_script) && bad_script != "") {
      File payloadFile = fs.open(bad_script, "r");
      if (payloadFile) {
        tft.setCursor(0, 40);
        tft.println("from file!");
        String lineContent = "";
        String Command = "";
        char Cmd[15];
        String Argument = "";
        String RepeatTmp = "";
        char ArgChar;
        bool ArgIsCmd;
        int cmdFail;

        Kb.releaseAll();
        tft.setTextSize(1);
        tft.setCursor(0, 0);
        tft.fillScreen(bruceConfig.bgColor);

        while (payloadFile.available()) {
          previousMillis = millis();
          if (check(SelPress)) {
            while (check(SelPress));
            options = {
              {"Continue",  [=]() { yield(); }},
              {"Main Menu", [=]() { returnToMenu = true; }},
            };

            loopOptions(options);

            if (returnToMenu) break;
            tft.setTextSize(FP);
          }
          lineContent = payloadFile.readStringUntil('\n');
          if (lineContent.endsWith("\r")) lineContent.remove(lineContent.length() - 1);

          ArgIsCmd = false;
          cmdFail = 0;
          RepeatTmp = lineContent.substring(0, lineContent.indexOf(' '));
          RepeatTmp = RepeatTmp.c_str();
          if (RepeatTmp == "REPEAT") {
            if (lineContent.indexOf(' ') > 0) {
              RepeatTmp = lineContent.substring(lineContent.indexOf(' ') + 1);
              if (RepeatTmp.toInt() == 0) {
                RepeatTmp = "1";
                tft.setTextColor(ALCOLOR);
                tft.println("REPEAT argument NaN, repeating once");
              }
            } else {
              RepeatTmp = "1";
              tft.setTextColor(ALCOLOR);
              tft.println("REPEAT without argument, repeating once");
            }
          } else {
            Command = lineContent.substring(0, lineContent.indexOf(' '));
            strcpy(Cmd, Command.c_str());
            if (lineContent.indexOf(' ') > 0) Argument = lineContent.substring(lineContent.indexOf(' ') + 1);
            else Argument = "";
            RepeatTmp = "1";
          }
          uint16_t i;
          for (i = 0; i < RepeatTmp.toInt(); i++) {
            char OldCmd[15];
            Argument = Argument.c_str();
            ArgChar = Argument.charAt(0);

            if (Argument == "F1" || Argument == "F2" || Argument == "F3" || Argument == "F4" || Argument == "F5" || Argument == "F6" || Argument == "F7" || Argument == "F8" || Argument == "F9" || Argument == "F10" || Argument == "F11" || Argument == "F12" || Argument == "DELETE" || Argument == "TAB" || Argument == "ENTER" || Argument == "ESCAPE" || Argument == "ESC") { ArgIsCmd = true; }

            restart:

            if (strcmp(Cmd, "REM") == 0) { Serial.println(" // " + Argument); } else { cmdFail++; }
            if (strcmp(Cmd, "DELAY") == 0) { delay(Argument.toInt() + dis(gen)); } else { cmdFail++; }
            if (strcmp(Cmd, "DEFAULTDELAY") == 0 || strcmp(Cmd, "DEFAULT_DELAY") == 0) delay(DEF_DELAY + dis(gen)); else { cmdFail++; }
            if (strcmp(Cmd, "STRING") == 0) { Kb.print(Argument); } else { cmdFail++; }
            if (strcmp(Cmd, "STRINGLN") == 0) { Kb.println(Argument); } else { cmdFail++; }
            if (strcmp(Cmd, "SHIFT") == 0) { if (Argument.length() > 0) { Kb.press(KEY_LEFT_SHIFT); if (!ArgIsCmd) { Kb.press(ArgChar); Kb.releaseAll(); } else { strcpy(OldCmd, Cmd); strcpy(Cmd, Argument.c_str()); goto restart; } } else { Kb.press(0xE1); Kb.releaseAll(); } } else { cmdFail++; }
            if (strcmp(Cmd, "ALT") == 0) { if (Argument.length() > 0) { Kb.press(KEY_LEFT_ALT); if (!ArgIsCmd) { Kb.press(ArgChar); Kb.releaseAll(); } else { strcpy(OldCmd, Cmd); strcpy(Cmd, Argument.c_str()); goto restart; } } else { Kb.press(0xE2); Kb.releaseAll(); } } else { cmdFail++; }
            if (strcmp(Cmd, "CTRL-ALT") == 0) { Kb.press(KEY_LEFT_ALT); Kb.press(KEY_LEFT_CTRL); if (!ArgIsCmd) { Kb.press(ArgChar); Kb.releaseAll(); } else { strcpy(OldCmd, Cmd); strcpy(Cmd, Argument.c_str()); goto restart; } } else { cmdFail++; }
            if (strcmp(Cmd, "CTRL-SHIFT") == 0) { Kb.press(KEY_LEFT_CTRL); Kb.press(KEY_LEFT_SHIFT); if (!ArgIsCmd) { Kb.press(ArgChar); Kb.releaseAll(); } else { strcpy(OldCmd, Cmd); strcpy(Cmd, Argument.c_str()); goto restart; } } else { cmdFail++; }
            if (strcmp(Cmd, "CTRL-GUI") == 0) { Kb.press(KEY_LEFT_CTRL); Kb.press(KEY_LEFT_GUI); if (!ArgIsCmd) { Kb.press(ArgChar); Kb.releaseAll(); } else { strcpy(OldCmd, Cmd); strcpy(Cmd, Argument.c_str()); goto restart; } } else { cmdFail++; }
            if (strcmp(Cmd, "ALT-SHIFT") == 0) { Kb.press(KEY_LEFT_ALT); Kb.press(KEY_LEFT_SHIFT); if (!ArgIsCmd) { Kb.press(ArgChar); Kb.releaseAll(); } else { strcpy(OldCmd, Cmd); strcpy(Cmd, Argument.c_str()); goto restart; } } else { cmdFail++; }
            if (strcmp(Cmd, "ALT-GUI") == 0) { Kb.press(KEY_LEFT_ALT); Kb.press(KEY_LEFT_GUI); if (!ArgIsCmd) { Kb.press(ArgChar); Kb.releaseAll(); } else { strcpy(OldCmd, Cmd); strcpy(Cmd, Argument.c_str()); goto restart; } } else { cmdFail++; }
            if (strcmp(Cmd, "GUI-SHIFT") == 0) { Kb.press(KEY_LEFT_GUI); Kb.press(KEY_LEFT_SHIFT); if (!ArgIsCmd) { Kb.press(ArgChar); Kb.releaseAll(); } else { strcpy(OldCmd, Cmd); strcpy(Cmd, Argument.c_str()); goto restart; } } else { cmdFail++; }
            if (strcmp(Cmd, "CTRL-ALT-SHIFT") == 0) { Kb.press(KEY_LEFT_ALT); Kb.press(KEY_LEFT_CTRL); Kb.press(KEY_LEFT_SHIFT); if (!ArgIsCmd) { Kb.press(ArgChar); Kb.releaseAll(); } else { strcpy(OldCmd, Cmd); strcpy(Cmd, Argument.c_str()); goto restart; } } else { cmdFail++; }
            if (strcmp(Cmd, "CTRL-ALT-GUI") == 0) { Kb.press(KEY_LEFT_ALT); Kb.press(KEY_LEFT_CTRL); Kb.press(KEY_LEFT_GUI); if (!ArgIsCmd) { Kb.press(ArgChar); Kb.releaseAll(); } else { strcpy(OldCmd, Cmd); strcpy(Cmd, Argument.c_str()); goto restart; } } else { cmdFail++; }
            if (strcmp(Cmd, "ALT-SHIFT-GUI") == 0) { Kb.press(KEY_LEFT_ALT); Kb.press(KEY_LEFT_SHIFT); Kb.press(KEY_LEFT_GUI); if (!ArgIsCmd) { Kb.press(ArgChar); Kb.releaseAll(); } else { strcpy(OldCmd, Cmd); strcpy(Cmd, Argument.c_str()); goto restart; } } else { cmdFail++; }
            if (strcmp(Cmd, "CTRL-SHIFT-GUI") == 0) { Kb.press(KEY_LEFT_CTRL); Kb.press(KEY_LEFT_SHIFT); Kb.press(KEY_LEFT_GUI); if (!ArgIsCmd) { Kb.press(ArgChar); Kb.releaseAll(); } else { strcpy(OldCmd, Cmd); strcpy(Cmd, Argument.c_str()); goto restart; } } else { cmdFail++; }
            if (strcmp(Cmd, "GUI") == 0 || strcmp(Cmd, "WINDOWS") == 0) { if (Argument.length() > 0) { Kb.press(KEY_LEFT_GUI); if (!ArgIsCmd) { Kb.press(ArgChar); Kb.releaseAll(); } else { strcpy(OldCmd, Cmd); strcpy(Cmd, Argument.c_str()); goto restart; } } else { Kb.press(0xE3); Kb.releaseAll(); } } else { cmdFail++; }
            if (strcmp(Cmd, "CTRL") == 0 || strcmp(Cmd, "CONTROL") == 0) { if (Argument.length() > 0) { Kb.press(KEY_LEFT_CTRL); if (!ArgIsCmd) { Kb.press(ArgChar); Kb.releaseAll(); } else { strcpy(OldCmd, Cmd); strcpy(Cmd, Argument.c_str()); goto restart; } } else { Kb.press(0xE0); Kb.releaseAll(); } } else { cmdFail++; }
            if (strcmp(Cmd, "ESC") == 0 || strcmp(Cmd, "ESCAPE") == 0) { Kb.press(KEY_ESC); Kb.releaseAll(); } else { cmdFail++; }
            if (strcmp(Cmd, "ENTER") == 0) { Kb.press(KEY_RETURN); Kb.releaseAll(); } else { cmdFail++; }
            if (strcmp(Cmd, "DOWNARROW") == 0) { Kb.press(KEY_DOWN_ARROW); Kb.releaseAll(); } else { cmdFail++; }
            if (strcmp(Cmd, "DOWN") == 0) { Kb.press(KEY_DOWN_ARROW); Kb.releaseAll(); } else { cmdFail++; }
            if (strcmp(Cmd, "LEFTARROW") == 0) { Kb.press(KEY_LEFT_ARROW); Kb.releaseAll(); } else { cmdFail++; }
            if (strcmp(Cmd, "LEFT") == 0) { Kb.press(KEY_LEFT_ARROW); Kb.releaseAll(); } else { cmdFail++; }
            if (strcmp(Cmd, "RIGHTARROW") == 0) { Kb.press(KEY_RIGHT_ARROW); Kb.releaseAll(); } else { cmdFail++; }
            if (strcmp(Cmd, "RIGHT") == 0) { Kb.press(KEY_RIGHT_ARROW); Kb.releaseAll(); } else { cmdFail++; }
            if (strcmp(Cmd, "UPARROW") == 0) { Kb.press(KEY_UP_ARROW); Kb.releaseAll(); } else { cmdFail++; }
            if (strcmp(Cmd, "UP") == 0) { Kb.press(KEY_UP_ARROW); Kb.releaseAll(); } else { cmdFail++; }
            if (strcmp(Cmd, "BREAK") == 0) { Kb.press(KEY_PAUSE); Kb.releaseAll(); } else { cmdFail++; }
            if (strcmp(Cmd, "CAPSLOCK") == 0) { Kb.press(KEY_CAPS_LOCK); Kb.releaseAll(); } else { cmdFail++; }
            if (strcmp(Cmd, "PAUSE") == 0) { Kb.press(KEY_PAUSE); Kb.releaseAll(); } else { cmdFail++; }
            if (strcmp(Cmd, "BACKSPACE") == 0) { Kb.press(KEYBACKSPACE); Kb.releaseAll(); } else { cmdFail++; }
            if (strcmp(Cmd, "END") == 0) { Kb.press(KEY_END); Kb.releaseAll(); } else { cmdFail++; }
            if (strcmp(Cmd, "HOME") == 0) { Kb.press(KEY_HOME); Kb.releaseAll(); } else { cmdFail++; }
            if (strcmp(Cmd, "INSERT") == 0) { Kb.press(KEY_INSERT); Kb.releaseAll(); } else { cmdFail++; }
            if (strcmp(Cmd, "NUMLOCK") == 0) { Kb.press(LED_NUMLOCK); Kb.releaseAll(); } else { cmdFail++; }
            if (strcmp(Cmd, "PAGEUP") == 0) { Kb.press(KEY_PAGE_UP); Kb.releaseAll(); } else { cmdFail++; }
            if (strcmp(Cmd, "PAGEDOWN") == 0) { Kb.press(KEY_PAGE_DOWN); Kb.releaseAll(); } else { cmdFail++; }
            if (strcmp(Cmd, "PRINTSCREEN") == 0) { Kb.press(KEY_PRINT_SCREEN); Kb.releaseAll(); } else { cmdFail++; }
            if (strcmp(Cmd, "SCROLLOCK") == 0) { Kb.press(KEY_SCROLL_LOCK); Kb.releaseAll(); } else { cmdFail++; }
            if (strcmp(Cmd, "MENU") == 0) { Kb.press(KEY_MENU); Kb.releaseAll(); } else { cmdFail++; }
            if (strcmp(Cmd, "F1") == 0) { Kb.press(KEY_F1); Kb.releaseAll(); } else { cmdFail++; }
            if (strcmp(Cmd, "F2") == 0) { Kb.press(KEY_F2); Kb.releaseAll(); } else { cmdFail++; }
            if (strcmp(Cmd, "F3") == 0) { Kb.press(KEY_F3); Kb.releaseAll(); } else { cmdFail++; }
            if (strcmp(Cmd, "F4") == 0) { Kb.press(KEY_F4); Kb.releaseAll(); } else { cmdFail++; }
            if (strcmp(Cmd, "F5") == 0) { Kb.press(KEY_F5); Kb.releaseAll(); } else { cmdFail++; }
            if (strcmp(Cmd, "F6") == 0) { Kb.press(KEY_F6); Kb.releaseAll(); } else { cmdFail++; }
            if (strcmp(Cmd, "F7") == 0) { Kb.press(KEY_F7); Kb.releaseAll(); } else { cmdFail++; }
            if (strcmp(Cmd, "F8") == 0) { Kb.press(KEY_F8); Kb.releaseAll(); } else { cmdFail++; }
            if (strcmp(Cmd, "F9") == 0) { Kb.press(KEY_F9); Kb.releaseAll(); } else { cmdFail++; }
            if (strcmp(Cmd, "F10") == 0) { Kb.press(KEY_F10); Kb.releaseAll(); } else { cmdFail++; }
            if (strcmp(Cmd, "F11") == 0) { Kb.press(KEY_F11); Kb.releaseAll(); } else { cmdFail++; }
            if (strcmp(Cmd, "F12") == 0) { Kb.press(KEY_F12); Kb.releaseAll(); } else { cmdFail++; }
            if (strcmp(Cmd, "TAB") == 0) { Kb.press(KEYTAB); Kb.releaseAll(); } else { cmdFail++; }
            if (strcmp(Cmd, "DELETE") == 0) { Kb.press(KEY_DELETE); Kb.releaseAll(); } else { cmdFail++; }
            if (strcmp(Cmd, "SPACE") == 0) { Kb.press(KEY_SPACE); Kb.releaseAll(); } else { cmdFail++; }

            if (ArgIsCmd) strcpy(Cmd, OldCmd);

            Kb.releaseAll();

            if (tft.getCursorY() > (tftHeight - LH)) {
              tft.setCursor(0, 0);
              tft.fillScreen(bruceConfig.bgColor);
            }

            if (cmdFail == 57) {
              tft.setTextColor(ALCOLOR);
              tft.print(Command);
              tft.println(" -> Not Supported, running as STRINGLN");
              if (Argument != "") {
                Kb.print(Command);
                Kb.print(" ");
                Kb.println(Argument);
              } else {
                Kb.println(Command);
              }
            } else {
              tft.setTextColor(bruceConfig.priColor);
              tft.print(Command);
            }
            if (Argument.length() > 0) {
              tft.setTextColor(TFT_WHITE);
              tft.println(Argument);
            } else tft.println();
            if (strcmp(Cmd, "REM") != 0) delay(DEF_DELAY + dis(gen));
          }
        }
        tft.setTextSize(FM);
        payloadFile.close();
        Serial.println("Finished badusb payload execution...");
      }
    } else {
      Serial.println("rick");
      tft.setCursor(0, 40);
      tft.println("rick");
      Kb.press(KEY_LEFT_GUI);
      Kb.press('r');
      Kb.releaseAll();
      delay(1000);
      Kb.print("https://www.youtube.com/watch?v=dQw4w9WgXcQ");
      Kb.press(KEY_RETURN);
      Kb.releaseAll();
    }

    delay(1000);
    Kb.releaseAll();
  }
}

bool kbChosen = false;

void chooseKb(const uint8_t *layout) {
  kbChosen = true;
  #if defined(USB_as_HID)
  Kb.begin(layout);
  #else
  mySerial.begin(CH9329_DEFAULT_BAUDRATE, SERIAL_8N1, BAD_RX, BAD_TX);
  delay(100);
  Kb.begin(mySerial, layout);
  #endif
}

void usb_setup() {
  Serial.println("BadUSB begin");
  tft.fillScreen(bruceConfig.bgColor);

  FS *fs;
  bool first_time = true;
  std::vector<String> bad_scripts;

NewScript:
  tft.fillScreen(bruceConfig.bgColor);
  String bad_script = "";
  bad_script = "/badpayload.txt";

  options = { };

  if (setupSdCard()) {
    options.push_back({"SD Card", [&]() { fs = &SD; }});
  }
  options.push_back({"LittleFS", [&]() { fs = &LittleFS; }});
  options.push_back({"Main Menu", [&]() { fs = nullptr; }});

  loopOptions(options);

  if (fs != nullptr) {
    bad_script = loopSD(*fs, true);
    tft.fillScreen(bruceConfig.bgColor);
    if (first_time) {
      options = {
        {"US International", [=]() { chooseKb(KeyboardLayout_en_US); }},
        {"Portuguese (Brazil)", [=]() { chooseKb(KeyboardLayout_pt_BR); }},
        {"Portuguese (Portugal)", [=]() { chooseKb(KeyboardLayout_pt_PT); }},
        {"French AZERTY", [=]() { chooseKb(KeyboardLayout_fr_FR); }},
        {"Spanish (Spain)", [=]() { chooseKb(KeyboardLayout_es_ES); }},
        {"Italian (Italy)", [=]() { chooseKb(KeyboardLayout_it_IT); }},
        {"English (UK)", [=]() { chooseKb(KeyboardLayout_en_UK); }},
        {"German (Germany)", [=]() { chooseKb(KeyboardLayout_de_DE); }},
        {"Swedish (Sweden)", [=]() { chooseKb(KeyboardLayout_sv_SE); }},
        {"Danish (Denmark)", [=]() { chooseKb(KeyboardLayout_da_DK); }},
        {"Hungarian (Hungary)", [=]() { chooseKb(KeyboardLayout_hu_HU); }},
        {"Turkish (Turkey)", [=]() { chooseKb(KeyboardLayout_tr_TR); }},
        {"Polish (Poland)", [=]() { chooseKb(KeyboardLayout_en_US); }},
      };
      loopOptions(options, false, true, "Keyboard Layout");

      #if defined(USB_as_HID)
      if (!kbChosen) Kb.begin();
      USB.begin();
      #else
      if (!kbChosen) {
        mySerial.begin(CH9329_DEFAULT_BAUDRATE, SERIAL_8N1, BAD_RX, BAD_TX);
        delay(100);
        Kb.begin(mySerial);
      }
      mySerial.write(0x00);
      while (mySerial.available() <= 0) {
        if (mySerial.available() <= 0) {
          displayTextLine("CH9329 -> USB");
          delay(200);
          mySerial.write(0x00);
        } else break;
        if (check(EscPress)) {
          displayError("CH9329 not found");
          return;
        }
      }
      #endif

      displayTextLine("Preparing");
      delay(2000);
      first_time = false;
    }
    displayWarning(String(BTN_ALIAS) + " to deploy", true);
    delay(200);
    bad_scripts.push_back(bad_script);
    key_input(*fs, bad_scripts);

    displayTextLine("Payload Sent", true);

    if (returnToMenu) return;
    goto NewScript;
  } else displayWarning("Canceled", true);
  returnToMenu = true;

  #if !defined(USB_as_HID)
  mySerial.end();
  Serial.begin(115200);
  #endif
}

void key_input_from_string(String text) {
  #if defined(USB_as_HID)
  Kb.begin();
  USB.begin();
  #else
  mySerial.begin(CH9329_DEFAULT_BAUDRATE, SERIAL_8N1, BAD_RX, BAD_TX);
  delay(100);
  Kb.begin(mySerial);
  #endif

  Kb.print(text.c_str());

  #if !defined(USB_as_HID)
  mySerial.end();
  #endif
}

#if defined(HAS_KEYBOARD)

void usb_keyboard() {
  drawMainBorder();
  tft.setTextSize(2);
  tft.setTextColor(bruceConfig.priColor);
  tft.drawString("Keyboard Started", tftWidth / 2, tftHeight / 2);
  options = {
    {"US Inter", [=]() { chooseKb(KeyboardLayout_en_US); }},
    {"PT-BR ABNT2", [=]() { chooseKb(KeyboardLayout_pt_BR); }},
    {"PT-Portugal", [=]() { chooseKb(KeyboardLayout_pt_PT); }},
    {"AZERTY FR", [=]() { chooseKb(KeyboardLayout_fr_FR); }},
    {"es-Espanol", [=]() { chooseKb(KeyboardLayout_es_ES); }},
    {"it-Italiano", [=]() { chooseKb(KeyboardLayout_it_IT); }},
    {"en-UK", [=]() { chooseKb(KeyboardLayout_en_UK); }},
    {"de-DE", [=]() { chooseKb(KeyboardLayout_de_DE); }},
    {"sv-SE", [=]() { chooseKb(KeyboardLayout_sv_SE); }},
    {"da-DK", [=]() { chooseKb(KeyboardLayout_da_DK); }},
    {"hu-HU", [=]() { chooseKb(KeyboardLayout_hu_HU); }},
    {"tr-TR", [=]() { chooseKb(KeyboardLayout_tr_TR); }},
    {"pl-PL", [=]() { chooseKb(KeyboardLayout_en_US); }},
    {"Main Menu", [=]() { returnToMenu = true; }},
  };

  loopOptions(options, false, true, "Keyboard Layout");
  if (returnToMenu) return;
  USB.begin();

  tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
  tft.setTextSize(FP);
  drawMainBorder();
  tft.setCursor(10, 28);
  tft.println("Usb Keyboard:");
  tft.drawCentreString("> " + String(KB_HID_EXIT_MSG) + " <", tftWidth / 2, tftHeight - 20, 1);
  tft.setTextSize(FM);
  String _mymsg = "";
  keyStroke key;
  while (1) {
    key = _getKeyPress();
    if (key.pressed) {
      if (key.enter) Kb.println();
      else if (key.del) Kb.press(KEYBACKSPACE);
      else {
        for (char k : key.word) {
          Kb.press(k);
        }
        for (auto k : key.modifier_keys) {
          Kb.press(k);
        }
      }
      if (key.fn && key.exit_key) break;

      Kb.releaseAll();

      String keyStr = "";
      for (auto i : key.word) {
        if (keyStr != "") {
          keyStr = keyStr + "+" + i;
        } else {
          keyStr += i;
        }
      }

      if (keyStr.length() > 0) {
        drawMainBorder(false);
        if (_mymsg.length() > keyStr.length()) tft.drawCentreString("                                  ", tftWidth / 2, tftHeight / 2, 1);
        tft.drawCentreString("Pressed: " + keyStr, tftWidth / 2, tftHeight / 2, 1);
        _mymsg = keyStr;
      }
      delay(200);
    }
  }
}
#endif
