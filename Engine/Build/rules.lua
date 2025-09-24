rule('corvus.windows.default')
  after_load(function (target)
    target:add('syslinks', 'kernel32', 'user32', 'gdi32', 'winspool', 'comdlg32', 'advapi32')
    target:add('syslinks', 'shell32', 'ole32', 'oleaut32', 'uuid', 'odbc32', 'odbccp32', 'comctl32')
    target:add('syslinks', 'comdlg32', 'setupapi', 'shlwapi')
    if not target:is_plat('mingw') then
      target:add('syslinks', 'strsafe')
    end
  end)