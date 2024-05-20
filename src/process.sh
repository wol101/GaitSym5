for file in *.cpp
do
  sed -i '/using namespace GaitSym;/namespace GaitSym {/g'  $file
  sed -i '$a} // namespace GaitSym' $file
  sed -i '$a\\n' $file
done
