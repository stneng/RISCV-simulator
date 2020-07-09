//
//  main.cpp
//  RISCV-simulator
//
//  Created by apple on 2020/7/7.
//  Copyright © 2020 apple. All rights reserved.
//

#include <iostream>
#include <cstdio>
unsigned int reg[32];unsigned char mem[100000000];
void load_data(){
    char s[23];int pos=0;
    while (scanf("%s",s)!=EOF) {
        if (s[0]=='@'){
            sscanf(s+1,"%x",&pos);
        }else{
            int tmp;
            sscanf(s,"%x",&tmp);
            mem[pos++]=tmp;
        }
    }
}
enum instruction{
    ERROR,
    ADD,SUB,SLL,SLT,SLTU,XOR,SRL,SRA,OR,AND, //R
    JALR,LB,LH,LW,LBU,LHU,ADDI,SLTI,SLTIU,XORI,ORI,ANDI,SLLI,SRLI,SRAI, //I
    SB,SH,SW, //S
    BEQ,BNE,BLT,BGE,BLTU,BGEU, //B
    LUI,AUIPC, //U
    JAL //J
};
struct ins{
    enum instruction ins;
    unsigned int rd,rs1,rs2,imm;
};
ins get_ins(unsigned int t){
    switch (t&0x7F){
        case 0x33:{
            // R
            unsigned int rd=(t>>7)&0x1F,rs1=(t>>15)&0x1F,rs2=(t>>20)&0x1F,funct3=(t>>12)&0x7,funct7=(t>>25)&0x7F;
            enum instruction inss=ERROR;
            switch (funct3) {
                case 0x0:
                    switch (funct7) {
                        case 0x0:inss=ADD;break;
                        case 0x20:inss=SUB;break;
                        default:break;
                    }
                    break;
                case 0x1:inss=SLL;break;
                case 0x2:inss=SLT;break;
                case 0x3:inss=SLTU;break;
                case 0x4:inss=XOR;break;
                case 0x5:
                    switch (funct7) {
                        case 0x0:inss=SRL;break;
                        case 0x20:inss=SRA;break;
                        default:break;
                    }
                    break;
                case 0x6:inss=OR;break;
                case 0x7:inss=AND;break;
                default:break;
            }
            return (ins){inss,rd,rs1,rs2,0u};
            break;
        }
        case 0x67:{
            // I
            unsigned int rd=(t>>7)&0x1F,rs1=(t>>15)&0x1F,funct3=(t>>12)&0x7,imm=0u;
            imm=(t>>20)&0x7FFF;if (t>>31) imm|=0xFFFFF800;
            enum instruction inss=ERROR;
            switch (funct3) {
                case 0x0:inss=JALR;break;
                default:break;
            }
            return (ins){inss,rd,rs1,0u,imm};
            break;
        }
        case 0x3:{
            // I
            unsigned int rd=(t>>7)&0x1F,rs1=(t>>15)&0x1F,funct3=(t>>12)&0x7,imm=0u;
            imm=(t>>20)&0x7FFF;if (t>>31) imm|=0xFFFFF800;
            enum instruction inss=ERROR;
            switch (funct3) {
                case 0x0:inss=LB;break;
                case 0x1:inss=LH;break;
                case 0x2:inss=LW;break;
                case 0x4:inss=LBU;break;
                case 0x5:inss=LHU;break;
                default:break;
            }
            return (ins){inss,rd,rs1,0u,imm};
            break;
        }
        case 0x13:{
            // I
            unsigned int rd=(t>>7)&0x1F,rs1=(t>>15)&0x1F,funct3=(t>>12)&0x7,imm=0u;
            imm=(t>>20)&0x7FFF;if (t>>31) imm|=0xFFFFF800;
            enum instruction inss=ERROR;
            switch (funct3) {
                case 0x0:inss=ADDI;break;
                case 0x2:inss=SLTI;break;
                case 0x3:inss=SLTIU;break;
                case 0x4:inss=XORI;break;
                case 0x6:inss=ORI;break;
                case 0x7:inss=ANDI;break;
                case 0x1:inss=SLLI;break;
                case 0x5:
                    switch (imm>>5) {
                        case 0x0:inss=SRLI;break;
                        case 0x20:inss=SRAI;break;
                        default:break;
                    }
                    break;
                default:break;
            }
            return (ins){inss,rd,rs1,0u,imm};
            break;
        }
        case 0x23:{
            // S
            unsigned int rs1=(t>>15)&0x1F,rs2=(t>>20)&0x1F,funct3=(t>>12)&0x7,imm=0u;
            imm=(t>>7)&0x1F;imm|=((t>>25)&0x3F)<<5;if (t>>31) imm|=0xFFFFF800;
            enum instruction inss=ERROR;
            switch (funct3) {
                case 0x0:inss=SB;break;
                case 0x1:inss=SH;break;
                case 0x2:inss=SW;break;
                default:break;
            }
            return (ins){inss,0u,rs1,rs2,imm};
            break;
        }
        case 0x63:{
            // B
            unsigned int rs1=(t>>15)&0x1F,rs2=(t>>20)&0x1F,funct3=(t>>12)&0x7,imm=0u;
            imm=((t>>8)&0xF)<<1;imm|=((t>>25)&0x3F)<<5;imm|=((t>>7)&0x1)<<11;if (t>>31) imm|=0xFFFFF000;
            enum instruction inss=ERROR;
            switch (funct3) {
                case 0x0:inss=BEQ;break;
                case 0x1:inss=BNE;break;
                case 0x4:inss=BLT;break;
                case 0x5:inss=BGE;break;
                case 0x6:inss=BLTU;break;
                case 0x7:inss=BGEU;break;
                default:break;
            }
            return (ins){inss,0u,rs1,rs2,imm};
            break;
        }
        case 0x37:{
            // U
            unsigned int rd=(t>>7)&0x1F,imm=0u;
            imm=((t>>12)&0xFFFFF)<<12;
            enum instruction inss=LUI;
            return (ins){inss,rd,0u,0u,imm};
            break;
        }
        case 0x17:{
            // U
            unsigned int rd=(t>>7)&0x1F,imm=0u;
            imm=((t>>12)&0xFFFFF)<<12;
            enum instruction inss=AUIPC;
            return (ins){inss,rd,0u,0u,imm};
            break;
        }
        case 0x6F:{
            // J
            unsigned int rd=(t>>7)&0x1F,rs1=(t>>15)&0x1F,imm=0u;
            imm=((t>>21)&0x3FF)<<1;imm|=((t>>20)&0x1)<<11;imm|=((t>>12)&0xFF)<<12;if (t>>31) imm|=0xFFF00000;
            enum instruction inss=JAL;
            return (ins){inss,rd,rs1,0u,imm};
            break;
        }
        default:{
            break;
        }
    }
    return (ins){ERROR,0u,0u,0u,0u};
}
struct intermediate{
    unsigned int origin_ins;
    enum instruction ins;
    unsigned int rd,rd_value,rs1_value,rs2_value,imm,mem_p,pc;
    int predictor;
    intermediate():origin_ins(0),ins(ERROR),rd(0),rd_value(0),rs1_value(0),rs2_value(0),imm(0),mem_p(0),pc(0),predictor(0){}
}IF_data,ID_data,EX_data,MEM_data;
int pc;bool ID_busy,EX_busy,MEM_busy,WB_busy;
int reg_busy[32],MEM_cycle;
int counter[128];int predictor_success,predictor_tot;
void IF(){
    if (ID_busy) return;
    unsigned int t=((unsigned int)mem[pc])|((unsigned int)mem[pc+1]<<8)|((unsigned int)mem[pc+2]<<16)|((unsigned int)mem[pc+3]<<24);
    IF_data=intermediate();
    IF_data.origin_ins=t;IF_data.pc=pc;
    pc+=4;
    ID_busy=true;
}
void ID(){
    if (EX_busy || !ID_busy) return;
    if (IF_data.origin_ins==0xFF00513) return;
    ins ins=get_ins(IF_data.origin_ins);
    if (ins.rs1 && reg_busy[ins.rs1]) return;
    if (ins.rs2 && reg_busy[ins.rs2]) return;
    ID_data=IF_data;
    ID_data.ins=ins.ins;ID_data.rd=ins.rd;ID_data.rs1_value=reg[ins.rs1];ID_data.rs2_value=reg[ins.rs2];ID_data.imm=ins.imm;
    EX_busy=true;ID_busy=false;
    if (ins.rd) reg_busy[ins.rd]++;
    switch (ID_data.ins) {
        case JAL:pc=ID_data.pc+ID_data.imm;break;
        case JALR:pc=(ID_data.rs1_value+ID_data.imm)&(~1);break;
        case BEQ:case BNE:case BLT:case BGE:case BLTU:case BGEU:
            ID_data.predictor=counter[(ID_data.pc>>2)&0x7F]&0x2;
            if (ID_data.predictor) pc=ID_data.pc+ID_data.imm;else pc=ID_data.pc+4;
            predictor_tot++;
            break;
        default:break;
    }
}
void EX(){
    if (MEM_busy || !EX_busy) return;
    EX_data=ID_data;
    switch (ID_data.ins) {
        case LUI:EX_data.rd_value=ID_data.imm;break;
        case AUIPC:EX_data.rd_value=ID_data.pc+ID_data.imm;break;
        case JAL:EX_data.rd_value=ID_data.pc+4;/*pc=ID_data.pc+ID_data.imm;ID_busy=false;*/break;
        case JALR:EX_data.rd_value=ID_data.pc+4;/*pc=(ID_data.rs1_value+ID_data.imm)&(~1);ID_busy=false;*/break;
        case BEQ:
            //if (ID_data.rs1_value==ID_data.rs2_value) pc=ID_data.pc+ID_data.imm;else pc=ID_data.pc+4;ID_busy=false;
            if (ID_data.rs1_value==ID_data.rs2_value){
                if (ID_data.predictor) predictor_success++;
                else {pc=ID_data.pc+ID_data.imm;ID_busy=false;}
                counter[(ID_data.pc>>2)&0x7F]=std::min(counter[(ID_data.pc>>2)&0x7F]+1,3);
            }else{
                if (!ID_data.predictor) predictor_success++;
                else {pc=ID_data.pc+4;ID_busy=false;}
                counter[(ID_data.pc>>2)&0x7F]=std::max(counter[(ID_data.pc>>2)&0x7F]-1,0);
            }
            break;
        case BNE:
            //if (ID_data.rs1_value!=ID_data.rs2_value) pc=ID_data.pc+ID_data.imm;else pc=ID_data.pc+4;ID_busy=false;
            if (ID_data.rs1_value!=ID_data.rs2_value){
                if (ID_data.predictor) predictor_success++;
                else {pc=ID_data.pc+ID_data.imm;ID_busy=false;}
                counter[(ID_data.pc>>2)&0x7F]=std::min(counter[(ID_data.pc>>2)&0x7F]+1,3);
            }else{
                if (!ID_data.predictor) predictor_success++;
                else {pc=ID_data.pc+4;ID_busy=false;}
                counter[(ID_data.pc>>2)&0x7F]=std::max(counter[(ID_data.pc>>2)&0x7F]-1,0);
            }
            break;
        case BLT:
            //if ((int)ID_data.rs1_value<(int)ID_data.rs2_value) pc=ID_data.pc+ID_data.imm;else pc=ID_data.pc+4;ID_busy=false;
            if ((int)ID_data.rs1_value<(int)ID_data.rs2_value){
                if (ID_data.predictor) predictor_success++;
                else {pc=ID_data.pc+ID_data.imm;ID_busy=false;}
                counter[(ID_data.pc>>2)&0x7F]=std::min(counter[(ID_data.pc>>2)&0x7F]+1,3);
            }else{
                if (!ID_data.predictor) predictor_success++;
                else {pc=ID_data.pc+4;ID_busy=false;}
                counter[(ID_data.pc>>2)&0x7F]=std::max(counter[(ID_data.pc>>2)&0x7F]-1,0);
            }
            break;
        case BGE:
            //if ((int)ID_data.rs1_value>=(int)ID_data.rs2_value) pc=ID_data.pc+ID_data.imm;else pc=ID_data.pc+4;ID_busy=false;
            if ((int)ID_data.rs1_value>=(int)ID_data.rs2_value){
                if (ID_data.predictor) predictor_success++;
                else {pc=ID_data.pc+ID_data.imm;ID_busy=false;}
                counter[(ID_data.pc>>2)&0x7F]=std::min(counter[(ID_data.pc>>2)&0x7F]+1,3);
            }else{
                if (!ID_data.predictor) predictor_success++;
                else {pc=ID_data.pc+4;ID_busy=false;}
                counter[(ID_data.pc>>2)&0x7F]=std::max(counter[(ID_data.pc>>2)&0x7F]-1,0);
            }
            break;
        case BLTU:
            //if (ID_data.rs1_value<ID_data.rs2_value) pc=ID_data.pc+ID_data.imm;else pc=ID_data.pc+4;ID_busy=false;
            if (ID_data.rs1_value<ID_data.rs2_value){
                if (ID_data.predictor) predictor_success++;
                else {pc=ID_data.pc+ID_data.imm;ID_busy=false;}
                counter[(ID_data.pc>>2)&0x7F]=std::min(counter[(ID_data.pc>>2)&0x7F]+1,3);
            }else{
                if (!ID_data.predictor) predictor_success++;
                else {pc=ID_data.pc+4;ID_busy=false;}
                counter[(ID_data.pc>>2)&0x7F]=std::max(counter[(ID_data.pc>>2)&0x7F]-1,0);
            }
            break;
        case BGEU:
            //if (ID_data.rs1_value>=ID_data.rs2_value) pc=ID_data.pc+ID_data.imm;else pc=ID_data.pc+4;ID_busy=false;
            if (ID_data.rs1_value>=ID_data.rs2_value){
                if (ID_data.predictor) predictor_success++;
                else {pc=ID_data.pc+ID_data.imm;ID_busy=false;}
                counter[(ID_data.pc>>2)&0x7F]=std::min(counter[(ID_data.pc>>2)&0x7F]+1,3);
            }else{
                if (!ID_data.predictor) predictor_success++;
                else {pc=ID_data.pc+4;ID_busy=false;}
                counter[(ID_data.pc>>2)&0x7F]=std::max(counter[(ID_data.pc>>2)&0x7F]-1,0);
            }
            break;
        case LB:case LH:case LW:case LBU:case LHU:case SB:case SH:case SW:EX_data.mem_p=ID_data.rs1_value+ID_data.imm;break;
        case ADDI:EX_data.rd_value=ID_data.rs1_value+ID_data.imm;break;
        case SLTI:EX_data.rd_value=(int)ID_data.rs1_value<(int)ID_data.imm;break;
        case SLTIU:EX_data.rd_value=ID_data.rs1_value<ID_data.imm;break;
        case XORI:EX_data.rd_value=ID_data.rs1_value^ID_data.imm;break;
        case ORI:EX_data.rd_value=ID_data.rs1_value|ID_data.imm;break;
        case ANDI:EX_data.rd_value=ID_data.rs1_value&ID_data.imm;break;
        case SLLI:EX_data.rd_value=ID_data.rs1_value<<(ID_data.imm&0x1F);break;
        case SRLI:EX_data.rd_value=ID_data.rs1_value>>(ID_data.imm&0x1F);break;
        case SRAI:EX_data.rd_value=(int)ID_data.rs1_value>>(ID_data.imm&0x1F);break;
        case ADD:EX_data.rd_value=ID_data.rs1_value+ID_data.rs2_value;break;
        case SUB:EX_data.rd_value=ID_data.rs1_value-ID_data.rs2_value;break;
        case SLL:EX_data.rd_value=ID_data.rs1_value<<(ID_data.rs2_value&0x1F);break;
        case SLT:EX_data.rd_value=(int)ID_data.rs1_value<(int)ID_data.rs2_value;break;
        case SLTU:EX_data.rd_value=ID_data.rs1_value<ID_data.rs2_value;break;
        case XOR:EX_data.rd_value=ID_data.rs1_value^ID_data.rs2_value;break;
        case SRL:EX_data.rd_value=ID_data.rs1_value>>(ID_data.rs2_value&0x1F);break;
        case SRA:EX_data.rd_value=(int)ID_data.rs1_value>>(ID_data.rs2_value&0x1F);break;
        case OR:EX_data.rd_value=ID_data.rs1_value|ID_data.rs2_value;break;
        case AND:EX_data.rd_value=ID_data.rs1_value&ID_data.rs2_value;break;
        default:break;
    }
    MEM_busy=true;EX_busy=false;
    MEM_cycle=0;
}
void MEM(){
    if (WB_busy || !MEM_busy) return;
    MEM_data=EX_data;
    switch (EX_data.ins) {
        case LB:case LH:case LW:case LBU:case LHU:case SB:case SH:case SW:
            if (MEM_cycle<3){
                MEM_cycle++;
            }else{
                switch (EX_data.ins) {
                    case LB:MEM_data.rd_value=(char)(unsigned int)mem[EX_data.mem_p];break;
                    case LH:MEM_data.rd_value=(short)(((unsigned int)mem[EX_data.mem_p])|((unsigned int)mem[EX_data.mem_p+1]<<8));break;
                    case LW:MEM_data.rd_value=((unsigned int)mem[EX_data.mem_p])|((unsigned int)mem[EX_data.mem_p+1]<<8)|((unsigned int)mem[EX_data.mem_p+2]<<16)|((unsigned int)mem[EX_data.mem_p+3]<<24);break;
                    case LBU:MEM_data.rd_value=(unsigned int)mem[EX_data.mem_p];break;
                    case LHU:MEM_data.rd_value=((unsigned int)mem[EX_data.mem_p])|((unsigned int)mem[EX_data.mem_p+1]<<8);break;
                    case SB:mem[EX_data.mem_p]=EX_data.rs2_value&0xFF;break;
                    case SH:mem[EX_data.mem_p]=EX_data.rs2_value&0xFF;mem[EX_data.mem_p+1]=(EX_data.rs2_value>>8)&0xFF;break;
                    case SW:mem[EX_data.mem_p]=EX_data.rs2_value&0xFF;mem[EX_data.mem_p+1]=(EX_data.rs2_value>>8)&0xFF;mem[EX_data.mem_p+2]=(EX_data.rs2_value>>16)&0xFF;mem[EX_data.mem_p+3]=(EX_data.rs2_value>>24)&0xFF;break;
                    default:break;
                }
                WB_busy=true;MEM_busy=false;
            }
            break;
        default:
            WB_busy=true;MEM_busy=false;
            break;
    }
}
void WB(){
    if (!WB_busy) return;
    switch (MEM_data.ins) {
        case SB:case SH:case SW:case BEQ:case BNE:case BLT:case BGE:case BLTU:case BGEU:break;
        default:
            if (MEM_data.rd){
                reg[MEM_data.rd]=MEM_data.rd_value;
                reg_busy[MEM_data.rd]--;
            }
            break;
    }
    WB_busy=false;
}
int main(int argc, const char * argv[]) {
    // insert code here...std::cout << "Hello, World!\n";
    load_data();
    int cycle=0;
    while (!(ID_busy && IF_data.origin_ins==0xFF00513 && !EX_busy && !MEM_busy && !WB_busy)){
        cycle++;
        WB();
        MEM();
        EX();
        ID();
        IF();
    }
    printf("%u\n",reg[10]&255u);
    fprintf(stderr,"cycle:%d\npredictor:%d/%d\n",cycle,predictor_success,predictor_tot);
    return 0;
}
