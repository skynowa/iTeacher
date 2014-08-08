[1;33mdiff --git a/Forms/TagsEditor.cpp b/Forms/TagsEditor.cpp[m
[1;33mindex 314f08a..62b33d2 100644[m
[1;33m--- a/Forms/TagsEditor.cpp[m
[1;33m+++ b/Forms/TagsEditor.cpp[m
[1;35m@@ -106,6 +106,7 @@[m [mTagsEditor::_construct()[m
 void[m
 TagsEditor::_destruct()[m
 {[m
[1;32m+[m[1;32m    _sqlNavigator.model()->select();[m
 }[m
 //-------------------------------------------------------------------------------------------------[m
 [m
[1;33mdiff --git a/QtLib b/QtLib[m
[1;33mindex 3cc4a6d..0d89b67 160000[m
[1;33m--- a/QtLib[m
[1;33m+++ b/QtLib[m
[1;35m@@ -1 +1 @@[m
[1;31m-Subproject commit 3cc4a6d4ecc1080fbed1e25fa62d042070278423[m
[1;32m+[m[1;32mSubproject commit 0d89b672c71e792d8aeafed7d03b98e0e967ce15[m
