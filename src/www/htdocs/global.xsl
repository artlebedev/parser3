<?xml version="1.0"?> 
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
<xsl:param name="param1"/>

  <xsl:template match="doc">
    <html>
    	<title>test3</title>
    	<xsl:apply-templates/>
    	<hr/>
    	param1=<xsl:value-of select="$param1"/>.
    </html>
  </xsl:template>

  <xsl:template match="important">
    <b><xsl:value-of select="."/></b>
  </xsl:template>
</xsl:stylesheet>
