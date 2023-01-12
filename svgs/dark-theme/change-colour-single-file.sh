file=$1
old_color=$2
new_color=$3

sed -i -e "s/fill:#${old_color}/fill:#${new_color}/g" $file
sed -i -e "s/stroke:#${old_color}/stroke:#${new_color}/g" $file

