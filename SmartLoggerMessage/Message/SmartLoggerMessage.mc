MessageIdTypedef=DWORD
LanguageNames=(Japanese=0x411:MSG00411)

MessageId=1
SymbolicName=EVENT_START_SERVICE
Language=Japanese
SMART Loggerサービスの稼動を開始しました。%r
FluentCatPath=%1%r
LogFilePath=%2%r
FluentdSensorName=%3%r
FluentdHostName=%4%r
Interval=%5%r
.

MessageId=2
SymbolicName=EVENT_STOP_SERVICE
Language=Japanese
SMART Loggerサービスの稼動を停止しました。%r
.

MessageId=3
SymbolicName=EVENT_START_SERVICE_ERROR
Language=Japanese
サービス開始処理実行中に「%1」にてエラーが発生しました。%r
.

MessageId=4
SymbolicName=EVENT_SENDSTATUSTOSCM_ERROR
Language=Japanese
SCMへのサービス状況（ステータス[%1], 進捗指標[%2]）の通知に失敗しました。%r
.

MessageId=10
SymbolicName=EVENT_GET_SMART_SUCCESS
Language=Japanese
SMART取得成功しました%r
ファイル=%1%r
情報数=%2%r
.

MessageId=11
SymbolicName=EVENT_GET_SMART_ERROR
Language=Japanese
SMART取得に失敗%r
ReadIdentifyError=%1%r
ReadSmartError=%2%r
ReadThresholdError=%3%r
ReadLogError=%4%r
.

MessageId=12
SymbolicName=EVENT_FILE_WRITE_ERROR
Language=Japanese
ファイル「%1」の作成に失敗%r
.

MessageId=20
SymbolicName=EVENT_SENDTO_FLUENTD_EVENT1
Language=Japanese
fluent-cat実行%r
Error=%1%r
.

MessageId=21
SymbolicName=EVENT_SENDTO_FLUENTD_EVENT
Language=Japanese
fluent-cat実行%r
Error=%1%r
.
