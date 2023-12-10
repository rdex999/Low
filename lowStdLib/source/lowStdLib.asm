; Compile with nasm -felf64 -o ../object/lowStdLib.obj lowStdLib.asm

%include "./printStr/printStr.asm"
%include "./printChar/printChar.asm"
%include "./printInt/printInt.asm"
%include "./printFloat32/printFloat32.asm"
%include "./print/print.asm"