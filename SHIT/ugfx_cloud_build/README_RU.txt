UGFX — сборка VST3 для FL Studio БЕЗ Visual Studio на вашем компьютере

ВАЖНО: в этом архиве пока исходный код и инструкция облачной сборки, а не готовый .vst3.
Плагин будет собран на облачном Windows runner GitHub Actions. Устанавливать Visual Studio,
CMake или Git на свой компьютер не нужно.

КАК ПОЛУЧИТЬ ГОТОВЫЙ ПЛАГИН:
1. Создайте аккаунт на https://github.com/ (если его ещё нет).
2. Создайте новый пустой репозиторий (New repository).
3. Загрузите в него ВСЕ файлы и папки из этого архива, включая скрытую папку .github.
   Удобнее всего: распаковать архив, открыть репозиторий на GitHub и выбрать
   Add file -> Upload files; перетащить содержимое папки.
4. Откройте вкладку Actions. Если GitHub попросит разрешить Actions, подтвердите.
5. Выберите workflow "Build UGFX VST3 for Windows" -> Run workflow.
6. Дождитесь зелёной отметки успешной сборки.
7. Откройте завершённый запуск workflow и скачайте artifact "UGFX-Windows-VST3".
8. Распакуйте скачанный artifact. Внутри будет папка UGFX.vst3.
9. Скопируйте UGFX.vst3 в:
   %LOCALAPPDATA%\Programs\Common\VST3\
   Если папки VST3 нет — создайте её. Копируйте всю папку UGFX.vst3 целиком.
10. Откройте FL Studio -> Options -> Manage plugins -> Find installed plugins.
    Найдите UGFX и добавьте эффект на Mixer insert.

ТРЕБОВАНИЯ: нужен интернет и аккаунт GitHub. Бесплатного GitHub Actions обычно достаточно
для такого небольшого проекта; лимиты зависят от типа аккаунта и настроек GitHub.

Состав эффектов: Flanger, Ghost, Underwater, Bitcrush, Reverb, Distortion, Delay,
Helicopter, Stutter, Speed, Pan, Mix.

Это самостоятельная реализация по мотивам описания/референса, не оригинальный коммерческий UGFX.
