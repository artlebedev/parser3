@CLASS
a

@OPTIONS
locals

@create[]
$self.one_1[]
$self.one_2[]
$self.one_3[]
$self.two_1[]
$self.two_2[]
$self.two_3[]

@one[][locals;one_1]
$one_1[one_1]	^rem{ local }
$one_2[one_2]	^rem{ local because of 'locals' }
$self.one_3[one_3]	^rem{ not local }

@two[][two_1]
$two_1[two_1]	^rem{ local }
$two_2[two_2]	^rem{ not local }
$self.two_3[two_3]	^rem{ not local }

@run[]
^self.one[]
^self.two[]
^if($self.three is "junction"){
	^self.three[]
}
^if($self.four is "junction"){
	^self.four[]
}