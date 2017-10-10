MessageIdTypedef=DWORD
LanguageNames=(Japanese=0x411:MSG00411)

MessageId=0x1
SymbolicName=EVENT_START_SERVICE
Language=Japanese
SMART Loggerサービスの稼動を開始しました。%r
FluentCatPath=%1%r
LogFilePath=%2%r
FluentdSensorName=%3%r
FluentdHostName=%4%r
Interval=%5%r
.

MessageId=0x2
SymbolicName=EVENT_STOP_SERVICE
Language=Japanese
SMART Loggerサービスの稼動を停止しました。%r
.

MessageId=0x3
SymbolicName=EVENT_START_SERVICE_ERROR
Language=Japanese
サービス開始処理実行中に「%1」にてエラーが発生しました。
.

MessageId=0x10
SymbolicName=EVENT_GET_SMART_ERROR
Language=Japanese
SMART取得に失敗
.

MessageId=0x11
SymbolicName=EVENT_FILE_WRITE_ERROR
Language=Japanese
ファイル「%1」の作成に失敗
.

MessageId=0x20
SymbolicName=EVENT_SENDTO_FLUENTD_EVENT1
Language=Japanese
fluent-cat実行%r
Error=%1
.

MessageId=0x21
SymbolicName=EVENT_SENDTO_FLUENTD_EVENT
Language=Japanese
fluent-cat実行%r
Error=%1%r
Json=%2
.

MessageId=0x7
SymbolicName=EVENT_SENDSTATUSTOSCM_ERROR
Language=Japanese
SCMへのサービス状況（ステータス[%1], 進捗指標[%2]）の通知に失敗しました。
.
