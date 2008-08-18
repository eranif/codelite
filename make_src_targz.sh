# export the current directory 
cur_rev=`svn info | grep Revision | awk '{print $2;}'`
curdir=`pwd`
rm -fr /tmp/codelite-${cur_rev}/

svn export . /tmp/codelite-1.0.${cur_rev}/
cp ./LiteEditor/svninfo.cpp /tmp/codelite-1.0.${cur_rev}/LiteEditor/
echo "Creating tar-ball"
cd /tmp/
tar cvfz ${curdir}/codelite-1.0.${cur_rev}.tar.gz codelite-1.0.${cur_rev}/*

