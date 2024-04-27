#include <stdio.h>


typedef enum {
    IN_SET, IN_ADDRHL, IN_MEM,
    IN_SWP, IN_ADD,    IN_NOT,
    IN_OR,  IN_JZ
} INSTRUCTION;
typedef enum {
    RA,  RB,   RC,
    RL,  RH,   RIL,
    RIH, RFLAG
} REGISTER;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int COUNT;
typedef struct {
    BYTE reg[8];
} state_data;

int main(int argc, char **argv) {
    FILE *fp;
    COUNT fsize;
    BYTE memory[0x10000];
    state_data state = {{0,0,0,0,0,0,0,0}};
    BYTE current, null, option, temp;
    INSTRUCTION instruction;
    REGISTER    reg;
    if(argc > 1) {
        fp = fopen(argv[1], "r");
        fsize = (fseek(fp, 0L, SEEK_END), ftell(fp));
        fseek(fp, 0L, SEEK_SET);
        fread(memory, 1, fsize, fp);
    }
    while(memory[0xFFFF] != 0xFF) {
        current = memory[state.reg[RIL] | (state.reg[RIH]<<8)];
        instruction = current & 7;
        reg         = (current>>3) & 7;
        null        = (current>>6) & 3;
        option      = 1;
        if(null==3) {
            if(!(++state.reg[RIL])) ++state.reg[RIH];
            continue;
        } else if(!null) {
            if(!(++state.reg[RIL])) ++state.reg[RIH];
            option = memory[state.reg[RIL] | (state.reg[RIH]<<8)];
        }
        printf("[%04x]\n", state.reg[RIL] | (state.reg[RIH]<<8));
        printf("%04x %02x\n", current, option);
        printf("<%02x>%02x %02x %02x\n", null, instruction, reg, option);
        printf(" A| %02x    B| %02x    C| %02x\n"
               " H| %02x    L| %02x   IH| %02x\n"
               "IL| %02x  FLG| %02x  HLT| %02x\n",
               state.reg[RA], state.reg[RB],
               state.reg[RC], state.reg[RH], state.reg[RL],
               state.reg[RIH], state.reg[RIL], state.reg[RFLAG],
               memory[0xFFFF]);
        switch(instruction) {
        case IN_SET:
            state.reg[reg] = option;
            if(!state.reg[reg]) state.reg[RFLAG] |= 1;
            else state.reg[RFLAG] &= 0xFE;
            printf("r%x = %02x\n", reg, option);
            break;
        case IN_ADDRHL:
            state.reg[RH] = state.reg[reg];
            state.reg[RL] = state.reg[option];
            printf("HL = r%xr%x\n", reg, option);
            break;
        case IN_MEM:
            if(null) {
                state.reg[reg] = memory[state.reg[RL] |
                    (state.reg[RH]<<8)];
                printf("r%x = [HL]\n", reg);
                break;
            }
            memory[state.reg[RL] | state.reg[RH]<<8] =
                state.reg[reg];
            printf("[HL] = r%x\n", reg);
            break;
        case IN_SWP:
            temp = state.reg[reg];
            state.reg[reg] = state.reg[option];
            state.reg[option] = temp;
            printf("(r%x, r%x) = (r%x, r%x)\n", reg, option,
                    option, reg);
            break;
        case IN_ADD:
            if(state.reg[reg] + state.reg[option] < state.reg[reg])
                state.reg[RFLAG] |= 2;
            else state.reg[RFLAG] &= 0xFD;
            state.reg[reg] += state.reg[option];
            if(!state.reg[reg]) state.reg[RFLAG] |= 1;
            else state.reg[RFLAG] &= 0xFE;
            printf("r%x += r%x\n", reg, option);
            break;
        case IN_NOT:
            if(~state.reg[reg] + option < ~state.reg[reg])
                state.reg[RFLAG] |= 2;
            else state.reg[RFLAG] &= 0xFD;
            state.reg[reg] = ~state.reg[reg] + option;
            if(!state.reg[reg]) state.reg[RFLAG] |= 1;
            else state.reg[RFLAG] &= 0xFE;
            printf("r%x = ~r%x + %02x\n", reg, reg, option);
            break;
        case IN_OR:
            state.reg[reg] |= state.reg[option];
            if(!state.reg[reg]) state.reg[RFLAG] |= 1;
            else state.reg[RFLAG] &= 0xFE;
            printf("r%x |= r%x\n", reg, option);
            break;
        case IN_JZ:
            if(!(state.reg[RFLAG]&1)) break;
            state.reg[RIH] = state.reg[RH];
            state.reg[RIL] = state.reg[RL];
            continue;
        }
        getchar();
        if(!(++state.reg[RIL])) ++state.reg[RIH];
    }
}


