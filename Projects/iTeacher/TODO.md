# TODO

## iTeacher

- [!] WordEditor - GT URL
- [ ] UI langs support - rm
- [ ] Config: URL to My Google Docs
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
- [ ] CMake: distrs (Win, Linux)
- [+] Deploy script

- [ ] [Move new words to DB](https://docs.google.com/spreadsheets/d/1m8uh17iOzbM5aj8NWxBxTZbrDobUbOsJepj_sH9Uz_A/edit)

- [+] Hint classes
- [+] Mute mode

## Voice

- [!] FIX: speech
- [ ] Voice on translate
- [ ] http://www.sfml-dev.org/

## Translation suppliers (Yandex)

- [ ] http://api.yandex.com/translate/doc/dg/reference/translate.xml
- [ ] https://tech.yandex.ru/translate/doc/dg/reference/translate-docpage/
- [ ] https://dictionary.cambridge.org/

## Port

- [ ] Qt 5 -> 6
- [ ] QtQuick (+ Android)

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

- [ ] Auto submit

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
