# TODO

## \see Translators

- StarDict
- https://github.com/yasirtug/ni-translate
- https://github.com/crow-translate/crow-translate
- https://github.com/soimort/translate-shell

## iTeacher

- [-] Plays at toolbar
- [+] MsgBox - show tag
- [ ] Extra `learn / marked` buttons
- [+] WordEditor - GoogleTranslate URL
- [+] Config: URL to My Google Docs [New words](https://docs.google.com/spreadsheets/d/1m8uh17iOzbM5aj8NWxBxTZbrDobUbOsJepj_sH9Uz_A/edit)
- [ ] Add example (small sentence) with using `term`
- [ ] Hint::_timeoutMs - by words number
- [+] UI langs support - rm
- [ ] Detail (full) translation
- [ ] Word status - Repeat, Marked, Learned
- [ ] DB: transcription - add
- [ ] Dictionaries ComboBox -> flat RadioButton
- [+] Translation order: ru -> en
- [ ] `QTableView` - Ru column at 1-st position (ru -> en)
- [ ] Auto-save new words
- [ ] Translation in place (replace text)
- [ ] UI: Kurs NBU (Android)
- [ ] Quick search
- [ ] WordEditor - translating on edit, FIX
- [?] CMake: distrs (Win, Linux)
- [+] Deploy script

- [+] Hint classes
- [+] Mute mode

## Voice

- [!] FIX: speech
- [+] Voice on translate
- [ ] http://www.sfml-dev.org/

## Translation suppliers (Yandex)

- [+] https://www.deepl.com/translator
- [ ] http://api.yandex.com/translate/doc/dg/reference/translate.xml
- [ ] https://tech.yandex.ru/translate/doc/dg/reference/translate-docpage/
- [ ] https://dictionary.cambridge.org/

## Port

- [+] Qt 5 -> 6
- [n/a] QtQuick (+ Android)

## FAQ

- [ ] https://github.com/AlexSnet/ASTranslator/
- [ ] http://code.google.com/p/quick-text-translator/
- [ ] http://code.google.com/p/lightlang/
- [ ] [StarDict](http://stardict-4.sourceforge.net/index_en.php)
- [ ] [Как упорядочить все, что я учу в одном документе?](https://www.youtube.com/watch?v=4lhSxVysohM)

## UI

- [ ] Cursor

```c++
QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
QApplication::restoreOverrideCursor();
```

- [?] Auto submit

```c++
// _model
if (_model != nullptr) {
    bool bRv = _model->submitAll();
    if (!bRv &&
        _model->lastError().text().contains("failed", Qt::CaseInsensitive))
    {
        // \see QtLib SqlRelationalTableModelEx::importCsv()

        // lastError(): QSqlError("19", "Unable to fetch row", "UNIQUE constraint failed: t_main.f_main_term")
        // qDebug() << qTRACE_VAR(lastError().text());
    } else {
        qCHECK_PTR(bRv, _model);
    }

    qPTR_DELETE(_model);
}
```
