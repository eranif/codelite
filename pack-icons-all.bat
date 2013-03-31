@ECHO OFF

cd codelite-icons
echo "Packing codelite-icons..."
call pack-icons.bat

cd ..\codelite-icons-fresh-farm
echo "Packing codelite-icons-fresh-farm..."
call pack-icons.bat

cd ..\codelite-icons-dark
echo "Packing codelite-icons-dark..."
call pack-icons.bat

cd ..