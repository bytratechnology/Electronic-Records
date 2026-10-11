/**
 * Project 11 — RFID attendance: Google Apps Script web app.
 *
 * Bound to a spreadsheet with two tabs:
 *   Students    columns: UID | Name | Class
 *   Attendance  columns: Time | UID | Name | Class | Device | Result
 *
 * Deploy as: Web app, Execute as "Me", Who has access "Anyone".
 * The shared secret below is what stops a stranger who finds the URL from
 * adding rows. Change it before deploying and never commit the real value.
 */

const SHARED_SECRET = 'CHANGE_ME';

function doPost(e) {
  let body;
  try {
    body = JSON.parse(e.postData.contents);
  } catch (error) {
    return json({ status: 'error', message: 'invalid JSON' });
  }

  if (body.secret !== SHARED_SECRET) {
    return json({ status: 'denied' });
  }

  const uid = String(body.uid || '').toUpperCase();
  if (!/^[0-9A-F]{8,20}$/.test(uid)) {
    return json({ status: 'error', message: 'invalid uid' });
  }

  const spreadsheet = SpreadsheetApp.getActiveSpreadsheet();
  const student = findStudent(spreadsheet.getSheetByName('Students'), uid);

  // scanned_at is 0 when the ESP32 clock had not synced; use arrival time then.
  const seconds = Number(body.scanned_at) || 0;
  const when = seconds > 0 ? new Date(seconds * 1000) : new Date();

  // Two readers may post at the same moment; the lock keeps rows from interleaving.
  const lock = LockService.getScriptLock();
  lock.waitLock(10000);
  try {
    spreadsheet.getSheetByName('Attendance').appendRow([
      when,
      uid,
      student ? student.name : '',
      student ? student.className : '',
      String(body.device || ''),
      student ? 'ok' : 'unknown card',
    ]);
  } finally {
    lock.releaseLock();
  }

  return student
    ? json({ status: 'ok', name: student.name })
    : json({ status: 'unknown' });
}

function findStudent(sheet, uid) {
  const rows = sheet.getDataRange().getValues();
  for (let i = 1; i < rows.length; i++) {  // row 0 is the header
    if (String(rows[i][0]).toUpperCase().trim() === uid) {
      return { name: rows[i][1], className: rows[i][2] };
    }
  }
  return null;
}

function json(payload) {
  return ContentService
    .createTextOutput(JSON.stringify(payload))
    .setMimeType(ContentService.MimeType.JSON);
}
