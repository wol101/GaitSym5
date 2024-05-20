for file in *.cpp
do
  sed -i 's/using namespace GaitSym;/namespace GaitSym {/g'  $file # search and replace
  #sed -i '$a} // namespace GaitSym' $file # apprend this line
  #sed -i '$a\\n' $file # append a blank line
done
