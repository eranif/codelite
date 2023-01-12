old_color=$1
new_color=$2
files=`ls *.svg`
for file in $files
do
    sed -i -e "s/fill:#${old_color}/fill:#${new_color}/g" $file
    sed -i -e "s/stroke:#${old_color}/stroke:#${new_color}/g" $file
done

