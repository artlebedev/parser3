@exception[origin;source;comment;type;code]
CGI_BIN_ERROR:
^if(def $source){$origin '$source'}
${comment}.
^if(def $type){type=$type}
^if(def $code){code=$code}

@auto_test[]
^BASE.auto_test[]
<li>cgi-bin

