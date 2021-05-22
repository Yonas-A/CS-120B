

unsigned get_keypad() {
    if     ( A0 && !A1 && !A2 && !A3 && !A4 && !A5 && !A6 && !A7) { return '0'; }
    else if(!A0 &&  A1 && !A2 && !A3 && !A4 && !A5 && !A6 && !A7) { return '1'; }
    else if(!A0 && !A1 &&  A2 && !A3 && !A4 && !A5 && !A6 && !A7) { return '2'; }
    else if(!A0 && !A1 && !A2 &&  A3 && !A4 && !A5 && !A6 && !A7) { return '3'; }
    else if(!A0 && !A1 && !A2 && !A3 &&  A4 && !A5 && !A6 && !A7) { return '4'; }
    else if(!A0 && !A1 && !A2 && !A3 && !A4 &&  A5 && !A6 && !A7) { return '5'; }
    else if(!A0 && !A1 && !A2 && !A3 && !A4 && !A5 &&  A6 && !A7) { return '6'; }
    else if(!A0 && !A1 && !A2 && !A3 && !A4 && !A5 && !A6 &&  A7) { return '7'; }

    else if( A0 &&  A1 && !A2 && !A3 && !A4 && !A5 && !A6 && !A7) { return '8'; }
    else if( A0 && !A1 &&  A2 && !A3 && !A4 && !A5 && !A6 && !A7) { return '9'; }
    else if( A0 && !A1 && !A2 &&  A3 && !A4 && !A5 && !A6 && !A7) { return 'A'; }
    else if( A0 && !A1 && !A2 && !A3 &&  A4 && !A5 && !A6 && !A7) { return 'B'; }
    else if( A0 && !A1 && !A2 &&  A3 && !A4 &&  A5 && !A6 && !A7) { return 'C'; }
    else if( A0 && !A1 && !A2 && !A3 &&  A4 && !A5 &&  A6 && !A7) { return 'D'; }
    else if( A0 && !A1 && !A2 && !A3 && !A4 && !A5 && !A6 &&  A7) { return '*'; }

    else if(!A0 &&  A1 &&  A2 && !A3 && !A4 && !A5 && !A6 && !A7) { return '#'; }
    else if(!A0 &&  A1 && !A2 &&  A3 && !A4 && !A5 && !A6 && !A7) { return '0'; }

    else { return '\0'; }
}


unsigned get_keypad_key() {
    if     ( A0 && !A1 && !A2 && !A3 && !A4 && !A5 && !A6 && !A7) { return '#'; }
    else if(!A0 &&  A1 && !A2 && !A3 && !A4 && !A5 && !A6 && !A7) { return '1'; }
    else if(!A0 && !A1 &&  A2 && !A3 && !A4 && !A5 && !A6 && !A7) { return '2'; }
    else if(!A0 && !A1 && !A2 &&  A3 && !A4 && !A5 && !A6 && !A7) { return '3'; }
    else if(!A0 && !A1 && !A2 && !A3 &&  A4 && !A5 && !A6 && !A7) { return '4'; }
    else if(!A0 && !A1 && !A2 && !A3 && !A4 &&  A5 && !A6 && !A7) { return '5'; }
    else if(!A0 && !A1 && !A2 && !A3 && !A4 && !A5 &&  A6 && !A7) { return '6'; }
    else if(!A0 && !A1 && !A2 && !A3 && !A4 && !A5 && !A6 &&  A7) { return '7'; }

    else { return '\0'; }
}


