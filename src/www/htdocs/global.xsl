<?xml version="1.0"?> 
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
<xsl:param name="param1">defparam1</xsl:param>

  <xsl:template match="people">
    <html>
    	<title><xsl:copy-of select="$param1"/></title>
    	<xsl:apply-templates/>
    </html>
  </xsl:template>

  <xsl:template match="name">
    <b><xsl:value-of select="."/></b>
  </xsl:template>
</xsl:stylesheet>
