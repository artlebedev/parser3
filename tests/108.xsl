<?xml version="1.0"?> 
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
<!--<xsl:output doctype-public ="-//W3C//DTD XHTML" indent="yes"/>-->

<xsl:param name="param1">defparam1</xsl:param>
<xsl:param name="param2">defparam2</xsl:param>

<xsl:variable name="var1">
<inside>in?<deep value="1"/></inside>
</xsl:variable>

	<xsl:template name="test">
		test return
	</xsl:template>

  <xsl:template match="people">
     <html>
     <head>z</head>
		param1=<xsl:value-of select="$param1"/><br/>
		param2=<xsl:value-of select="$param2"/><br/>

		test return: =<xsl:copy-of select="$var1"/>=
		 <something background=""/>

    	<xsl:apply-templates/>
    	<hr/>
    	<xsl:call-template name="test"/>
     </html>	
  </xsl:template>

  <xsl:template match="name">
    <b><xsl:value-of select="."/></b>
  </xsl:template>
</xsl:stylesheet>
