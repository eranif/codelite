# export the current directory 
cur_rev=`svn info | grep Revision | awk '{print $2;}'`
curdir=`pwd`
rm -fr /tmp/codelite-src-${cur_rev}/

svn export . /tmp/codelite-src-${cur_rev}/
cp ./LiteEditor/svninfo.cpp /tmp/codelite-src-${cur_rev}/LiteEditor/
echo "Creating tar-ball"
cd /tmp/
tar cvfz ${curdir}/codelite-1.0.${cur_rev}-src.tar.gz codelite-src-${cur_rev}/*

