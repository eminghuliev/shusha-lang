fn test(argv: str) -> void {
    let number: u32 = 1;
    let arg1: u32 = 1;
    let arg2: str = argv;
    let arg3: u32 = 34;
    asm("syscall":
    in("rax") = number,
    in("rdi") = arg1,
    in("rsi") = arg2,
    in("rdx") = arg3,
    clob("rcx", "r11", "memory")
    );
}


fn main() -> void {
    let emin: str = "First hello world\n";
    test(emin);
}
