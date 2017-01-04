output_file=`ls -l *.exe |awk '{print $9;}'`
output_file_7z="${output_file%.*}"
output_file_7z=${output_file_7z}.7z
7za a ${output_file_7z} ${output_file}
scp ${output_file_7z} root@codelite.org:/var/www/html/downloads/codelite/wip
ssh root@codelite.org 'chown -R www-data:www-data /var/www/html/downloads/codelite/wip'

