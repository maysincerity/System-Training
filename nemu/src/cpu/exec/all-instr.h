#include "cpu/exec.h"

make_EHelper(mov);

make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);

make_EHelper(call);
make_EHelper(push);
make_EHelper(pop);
make_EHelper(sub);
make_EHelper(xor);
make_EHelper(ret);

//arith
make_EHelper(add);
make_EHelper(cmp);
make_EHelper(inc);
make_EHelper(adc);
make_EHelper(imul2);
make_EHelper(idiv);
make_EHelper(dec);
make_EHelper(mul);
make_EHelper(imul1);
make_EHelper(sbb);
make_EHelper(div);
make_EHelper(neg);

//cc

//control
make_EHelper(jmp);
make_EHelper(jcc);
make_EHelper(call_rm);
make_EHelper(jmp_rm);

//data-mov
make_EHelper(lea);
make_EHelper(leave);
make_EHelper(movzx);
make_EHelper(cltd);
make_EHelper(movsx);
make_EHelper(cwtl);

//logic
make_EHelper(and);
make_EHelper(setcc);
make_EHelper(or);
make_EHelper(test);
make_EHelper(sar);
make_EHelper(shl);
make_EHelper(shr);
make_EHelper(not);
make_EHelper(rol);

//prefix

//relop

//special
make_EHelper(nop);

//system
make_EHelper(in);
make_EHelper(out);
