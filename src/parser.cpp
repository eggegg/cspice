Parser::Parser(const char * fname = NULL) {
   extern FILE * yyin ;
   extern int yyparse() ;
   extern int parseErrorCount ;

   if (fname != NULL) {
      if (!(yyin = fopen(fname, "r"))) {
         cerr << '"' << fname << '"' << " can't be opened." << endl ;
      }
   }

   currentParser = this ;

   if (yyparse() != 0 || parseErrorCount != 0) {

   }
}

void Parser::addFreqSwap(double start , double end , int pointPerDecade , const char * fname) {
   SimulateConfig single;
   single.start = start;
   single.end = end;
   single.step = pointPerDecade;
   single.filename = string(fname);
   single.type = FREQ;
   // TODO: need to mention source
   config.push_back(single);
}

void Parser::addTimeSwap(double start , double end , double step , const char * fname) {
   SimulateConfig single;
   single.start = start;
   single.end = end;
   single.step = step;
   single.filename = string(fname);
   single.type = TIME;
   config.push_back(single);
}

void Parser::setRLC(Element * element , int n1 , int n2) {
   Node * node1 = circuit.getNodeById(n1) ;
   Node * node2 = circuit.getNodeById(n2) ;

   circuit.pushElement(element) ;
   node1->setConnect(node2 , element) ;
   node2->setConnect(node1 , element) ;
}

void Parser::addR(const char * name , int n1 , int n2 , double v) {
   this->setRLC(new Resistor(name , v) , n1 , n2) ;
}

void Parser::addL(const char * name , int n1 , int n2 , double v) {
   this->setRLC(new Inductor(name , v) , n1 , n2) ;
}

void Parser::addC(const char * name , int n1 , int n2 , double v) {
   this->setRLC(new Capacitor(name , v) , n1 , n2) ;
}

void Parser::addV(const char * name , int n1 , int n2 , double v1 , double v2) {
   if(n1 == n2) {
      if(v1 != 0.0 || v2 != 0.0) {
         throw ParseError("Illigal voltage source: " + string(name)) ;
      }
   }

   Node * node1 = circuit.getNodeById(n1) ;
   Node * node2 = circuit.getNodeById(n2) ;

   Element * element = new VSRC(name , v1 , v2 , 1) ;
   Element * rev_ele = new VSRC(name , v1 , v2 , -1) ;

   circuit.pushElement(element) ;
   circuit.pushElement(rev_ele) ;
   circuit.pushSource(pair<Element*,Element*>(element, rev_ele));

   node1->setConnect(node2 , element);
   node2->setConnect(node1 , rev_ele);
}

void Parser::addI(const char * name , int n1 , int n2 , double v1 , double v2) {
   if(n1 == n2) {
      return ;
   }

   Node * node1 = circuit.getNodeById(n1) ;
   Node * node2 = circuit.getNodeById(n2) ;

   Element * element = new ISRC(name , v1 , v2 , 1) ;
   Element * rev_ele = new ISRC(name , v1 , v2 , -1) ;

   circuit.pushElement(element) ;
   circuit.pushElement(rev_ele) ;
   circuit.pushSource(pair<Element*,Element*>(element, rev_ele));

   node1->setConnect(node2 , element);
   node2->setConnect(node1 , rev_ele);
}

void Parser::addG(const char * name , int n1 , int n2 , int n3 , int n4 , double v) {
   Element * element = new VCCS(name , v , 1) ;
   Element * rev_ele = new VCCS(name , v ,-1) ;

   Node * node1 = circuit.getNodeById(n1) ;
   Node * node2 = circuit.getNodeById(n2) ;
   Node * node3 = circuit.getNodeById(n3) ;
   Node * node4 = circuit.getNodeById(n4) ;

   circuit.pushElement(element) ;
   circuit.pushElement(rev_ele) ;

   if(n3 != n2) node3->setConnect(node2 , element) ;
   if(n3 != n1) node3->setConnect(node1 , rev_ele) ;
   if(n4 != n2) node4->setConnect(node2 , rev_ele) ;
   if(n4 != n1) node4->setConnect(node1 , element) ;
}

void Parser::addOut(int n1 , int n2) {
   circuit.outputHighId = n1 ;
   circuit.outputLowId = n2 ;
}

