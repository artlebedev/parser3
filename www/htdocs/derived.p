@USE
/base.p

@CLASS
derived

@BASE
base

@auto[]
$aaa[1]

@create[aderived_param]
^BASE:create[base create actual param]
$derived_var[$aderived_param]

@test[][aaa]
$aaa[xxx]
^$CLASS.aaa=$CLASS.aaa<br>
^$aaa=$aaa<br>
^$derived_var=$derived_var<br>
^BASE:test[]

@test2[]
derived_test2<br>
^$field=$field<br>
#^shit[]

@includet[]
derived_includet<br>
^include[some.p]
^self.included_method[2]

@inclass_process_test[][ipt]
$ipt[3]
#^process[$MAIN:CLASS]{^^eval($ipt*2)}
#^process[$MAIN:CLASS]{^^test2[]}
$field[derived field value]
^process{^^test2[]}

