<?xml version="1.0"?> 
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
<!--<xsl:output doctype-public ="-//W3C//DTD XHTML" indent="yes"/>-->

<xsl:param name="param1">defparam1</xsl:param>

  <xsl:template match="people">
     <z>
   		para1=<xsl:copy-of select="$param1"/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/>
    	<xsl:apply-templates/>
     </z>	
  </xsl:template>

  <xsl:template match="name">
    <b><xsl:value-of select="."/></b>
  </xsl:template>
</xsl:stylesheet>
