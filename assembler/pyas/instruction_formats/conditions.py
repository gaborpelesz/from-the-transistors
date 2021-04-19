condition_to_int = {
    "al": 0b1110,
    "": 0b1110
}

def get_condition_code(op, cond):
    # operations with fix consequences
    if op.lower() in ["cmp", "cmn", "tst", "teq"]:
        S = 0b1
    elif cond and len(cond) == 3 and cond[-1].lower() == 's':
        S = 0b1
        cond = cond[:-1]
    else:
        S = 0b0
    try:
        cond = condition_to_int[cond.lower()]
    except KeyError:
        raise Exception(f"Unsupported/Unimplemented condition code: {cond}")
    
    return cond, S