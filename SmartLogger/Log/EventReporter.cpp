#include "StdAfx.h"
#include "StrArray.h"
#include "ReportEventData.h"
#include "EventReporter.h"

/**
 * @brief  オブジェクトの構築とともにフィールドへの値の割り当てを行う
 *
 * @author kumagai
 *
 * @param  sourceName イベントログのイベントソースの名前
 */
CEventReporter::CEventReporter(const TCHAR * sourceName)
	: sourceName(sourceName)
{
	// 何もしない。
}

/**
 * @brief  イベントログ使用準備を行う
 *
 * @author kumagai
 *
 * @retval TRUE 成功
 * @retval FALSE 失敗
 */
BOOL CEventReporter::Open(void)
{
	handle = ::RegisterEventSource(NULL, sourceName);

	return handle != NULL;
}

/**
 * @brief  イベントログ使用終了処理を行う
 *
 * @author kumagai
 *
 * @retval TRUE 成功
 * @retval FALSE 失敗
 */
BOOL CEventReporter::Close(void)
{
	return ::DeregisterEventSource(handle);
}

/**
 * @brief  イベントをレポートする
 *
 * data は本関数内で解放するため、呼び出し側での解放の必要なし。
 *
 * @author kumagai
 *
 * @param  data 対象データ
 *
 * @retval TRUE 成功
 * @retval FALSE 失敗
 */
BOOL CEventReporter::ReportEvent(CReportEventData * data)
{
	return
		::ReportEvent(
			handle,
			data->type,
			data->category,
			data->eventId,
			data->userSid,
			(WORD)data->strings->GetSize(),
			(DWORD)data->binarySize,
			(LPCTSTR *)data->strings->GetPointers(),
			(LPVOID)data->binary);
}
