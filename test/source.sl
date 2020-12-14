fn test() -> void 
{
    // declare variable
    let mut x: i8 = 1;
    let mut a: i32 = x;
    let mut y: *i8 = &x;
    let mut z: *i8 = y;
    let mut f: *i8 = y;
    z = f;
}
