# TODO

## Main

- [ ] Config: URL to My Google Docs
- [ ] Detail (full) translation
- [ ] Qt 5 -> 6
- [ ] Word status - Repeat, Marked, Learned
- [+] QLabel with Mouse actions
- [ ] Random term on startup
- [ ] Trainer (UI + Console) add
- [ ] UI langs support - rm

- [ ] DB: transcription - add
- [ ] Dictionaries ComboBox -> flat RadioButton
- [ ] Translation order: ru -> en
- [ ] `QTableView` - Ru column at 1-st position (ru -> en)
- [ ] Pin translator on top
- [ ] Auto-save new words
- [ ] Translation in place (replace text)
- [ ] UI: Kurs NBU (Android)
- [ ] Quick search
- [ ] WordEditor - translating on edit, FIX
- [ ] CMake: distrs (Win, Linux)

- [ ] [Move new words to DB](https://docs.google.com/spreadsheets/d/1m8uh17iOzbM5aj8NWxBxTZbrDobUbOsJepj_sH9Uz_A/edit)

- [+] Hint classes
- [+] Mute mode

## Voice

- [!] `QSound`
- [ ] FIX: voice feature
- [ ] Voice on translate
- [ ] `QSound`
- [ ] http://www.sfml-dev.org/

## Trainer

- https://bitbucket.org/skynowa/teacher_vcl/src/2dfc86d6f080c17ff60b0f68fd8828c26af0ec5f/Forms/uDesk.h?at=master&fileviewer=file-view-default
- https://bitbucket.org/skynowa/teacher_vcl/src/2dfc86d6f080c17ff60b0f68fd8828c26af0ec5f/Forms/uDesk.cpp?at=master&fileviewer=file-view-default
- Show "Trainer" desk when termin exists

## Translation suppliers (Yandex)

- [ ] http://api.yandex.com/translate/doc/dg/reference/translate.xml
- [ ] https://tech.yandex.ru/translate/doc/dg/reference/translate-docpage/
- [ ] https://dictionary.cambridge.org/

## Port

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