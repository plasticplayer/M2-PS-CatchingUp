//
//  main.cpp
//  server
//
//  Created by maxime max on 04/03/2015.
//  Copyright (c) 2015 Maxime Leblanc. All rights reserved.
//

#include <iostream>
#include "Udp.h"

using namespace std;

int main(int argc, const char * argv[]) {
    // insert code here...
    cout << "Start Server" << endl;

    Udp* udp = new Udp(1902);
    udp->listenner();


    return 0;
}
