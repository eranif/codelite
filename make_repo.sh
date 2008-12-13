################################################################################
## This file is part of CodeLite IDE and is released 
## under the terms of the GNU General Public License as published by  
##    the Free Software Foundation; either version 2 of the License, or     
##    (at your option) any later version.                              
################################################################################


#########################################################################################################################
##1. Install Apache and create the folder debs under its htdocs directory
##2. Copy the .deb file to /usr/local/apache2/htdocs/debs
##3. Run the following command from /usr/local/apache2/htdocs/debs: 
##	/usr/bin/dpkg-scanpackages . /dev/null > Packages
##4. Compress the Packages file to dists/gutsy/restricted/binary-i386/Packages.gz
##5. Add the following entry to your /etc/apt/sources.list: deb http://eran-linux:/debs  gutsy restricted (replace eran-linux with your IP/server name)


## base script directory
base_dir=`pwd`

## repository directory
repo_dir="/var/www/debs/"

parseCommandLine()
{
    case $1 in
        "-d")   cd $base_dir
                echo "removing old Debian packages..."
                findFiles $base_dir -v ubuntu
                rmFiles
                findFiles $repo_dir -v ubuntu
                rmFiles
                echo "Creating new Debian package..."
                $base_dir/make_deb.sh -d
                echo "Copying Debian package to /usr/local/apache2/htdocs/debs/"
                findFiles $base_dir -v ubuntu
                mkdir $repo_dir/debian
                cp $list $repo_dir/debian
                echo "changing directory to $repo_dir"
                cd $repo_dir/debian
                echo "Running: /usr/bin/dpkg-scanpackages tool..."
                /usr/bin/dpkg-scanpackages . /dev/null > Packages
                echo "Compressing Packages..."
                gzip Packages
                echo "Moving Packages.gz to dist directory"
                ## moving the Debian package to repository dir
                mv *.deb ../
                mv Packages.gz ../dists/lenny/restricted/binary-i386/Packages.gz
                cd ../
                rm -rf debian/
                echo "Done";;
        
        "-u")   cd $base_dir
                echo "removing old Ubuntu packages..."
                findFiles $base_dir "" ubuntu
                rmFiles
                findFiles $repo_dir "" ubuntu
                rmFiles
                echo "Creating new Ubuntu package..."
                $base_dir/make_deb.sh -u
                echo "Copying Ubuntu package to /usr/local/apache2/htdocs/debs/"
                findFiles $base_dir "" ubuntu
                mkdir $repo_dir/ubuntu
                cp $list $repo_dir/ubuntu
                echo "changing directory to $repo_dir"
                cd $repo_dir/ubuntu
                echo "Running: /usr/bin/dpkg-scanpackages tool..."
                /usr/bin/dpkg-scanpackages . /dev/null > Packages
                echo "Compressing Packages..."
                gzip Packages
                echo "Moving Packages.gz to dist directory"
                ## moving the Ubuntu package to repository dir
                mv *.deb ../
                mv Packages.gz ../dists/gutsy/restricted/binary-i386/Packages.gz
                cd ../
                rm -rf ubuntu/
                echo "Done";;
        
        "-a")   parseCommandLine -u
                parseCommandLine -d;;
                
        "--help" | "-h")    usage
                            exit 0;;
        *   )   usage
                exit 1;;
    esac
}

## Show help
usage()
{
    cat <<EOF
Usage: 
  `basename $0` [options]

Options:
  -h | --help   this help screen
  -d            make deb-package for Debian repository and place it there
  -u            make deb-package for Ubuntu repository and place it there
  -a            make both deb-packages and place them to their repository paths
  
EOF
}

## Find files in the given directory $1
## with given options $2 and $3
findFiles()
{
    list="`find $1 -maxdepth 1 -name \"*.deb\" | grep $2 \"$3\"`"
}

## Remove files specified in $list
rmFiles()
{
    for debfile in $list
    do
        rm -f $debfile
    done
}

parseCommandLine $1
exit 0