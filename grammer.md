
ÎÄ·¨£º


1.program -> declaration-list

2.declaration-list -> declaration-list declaration | declaration		/* ×óµÝ¹é */

3.declaration -> var-declaration | fun-declaration

4.var-declaration -> type-specifier ID; | type-specifier ID[num]; 

5.type-specifier -> int | void | char | pint | pchar 

6.fun-declaration ->  type-specifier ID ( params ) |  compound-stmt

7.params -> params-list |  void	| empty

8.param-list -> param-list, param | param		/* ×óµÝ¹é */

9.param -> type-specifier ID | type-specifier ID [  ]  

10.compound-stmt ->  { 	local-declaration  statement-list 	}

11.local-declaration->  local-declaration  var-declaration  |  empty	/* ×óµÝ¹é */
 
12.statement-list -> statement-list statement | empty	   /* ×óµÝ¹é */

13.statement -> expression-stmt | compound-stmt | selection-stmt | iteration-stmt | return-stmt

14.expression-stmt -> expression; |  ;

15.selection-stmt ->   if ( expression ) statement  | if( expression ) statement else statement

16.iteration-stmt -> while ( expression ) statement  | for ( expression; expression; expression ) statement

17.return-stmt -> return ; | return expression;

18.expression-> var = expression | simple-expression

19.var -> ID | ID[expression]  | unaryop ID | unaryop ID[expression] 

20.unaryop ->  * | & | ! 		/*  new add   */

21.simple-expression -> additive-expression relop additive-expression | additive-expression 

21.relop  ->   <= | < | > | >= | == |  !=		

22.additive-expression ->  additive-expression  addop term | term				/* ×óµÝ¹é */

23.addop -> + | -

24.term -> term mulop factor | factor				/* ×óµÝ¹é */

25.mulop ->  *  |   /

26.factor -> ( expression ) | var | call | NUM

27.call -> ID ( args )

28.args -> arg-list | empty

29.arg-list -> arg-list, expression | expression			/* ×óµÝ¹é */
