<?xml version="1.0"?> 
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
<xsl:output doctype-public ="-//W3C//DTD XHTML" indent="yes"/>

<xsl:param name="param1">defparam1</xsl:param>

	<xsl:template name="test">
		test return
	</xsl:template>

  <xsl:template match="people">
     <html>
     <head>z</head>
   		para1=<xsl:copy-of select="$param1"/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/>
    	<xsl:apply-templates/>
    	<hr/>
    	<xsl:call-template name="test"/>
     </html>	
  </xsl:template>

  <xsl:template match="name">
    <b><xsl:value-of select="."/></b>
  </xsl:template>
</xsl:stylesheet>
