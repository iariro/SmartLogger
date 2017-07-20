#include "StdAfx.h"
#include "StrArray.h"
#include "ReportEventData.h"

/**
 * @brief  オブジェクトの構築とともにフィールドへの値の割り当てを行う。
 * （引数の少ない簡易版）
 *
 * @author kumagai
 *
 * @param  type     イベントの種類
 * @param  category イベントの分類
 * @param  eventId  イベント識別子
 */
CReportEventData::CReportEventData(WORD type, WORD category, DWORD eventId)
	:	type(type), category(0), eventId(eventId), userSid(0),
		binary(NULL), binarySize(0)
{
	strings = new CCharPtrArray();
}

/**
 * @brief  オブジェクトの構築とともにフィールドへの値の割り当てを行う。
 *
 * 文字列データは strings メンバへの直接アクセスにより行う。
 *
 * @author kumagai
 *
 * @param  type       イベントの種類
 * @param  category   イベントの分類
 * @param  eventId    イベント識別子
 * @param  userSid    ユーザーセキュリティ識別子
 * @param  binary     バイナリデータのアドレス
 * @param  binarySize バイナリデータのサイズ（バイト数）
 */
CReportEventData::CReportEventData(WORD type, WORD category, DWORD eventId,
	PSID userSid, BYTE * binary, DWORD binarySize)
	:	type(type), category(0), eventId(eventId), userSid(0),
		binary(binary), binarySize(binarySize)
{
	strings = new CCharPtrArray();
}

/**
 * @brief  オブジェクトの破棄とともに集約していた strings の解放を行う。
 *
 * @author kumagai
 */
CReportEventData::~CReportEventData()
{
	delete strings;
}
