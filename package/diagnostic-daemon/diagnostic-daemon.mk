################################################################################
#
# diagnostic-daemon
#
################################################################################

DIAGNOSTIC_DAEMON_VERSION = 0.2.0
DIAGNOSTIC_DAEMON_SITE = $(BR2_EXTERNAL_diagnostic_daemon_PATH)/app
DIAGNOSTIC_DAEMON_SITE_METHOD = local

define DIAGNOSTIC_DAEMON_BUILD_CMDS
$(TARGET_CC) $(TARGET_CFLAGS) -Wall -Wextra -Werror -std=c11 \
-I$(@D)/include \
-o $(@D)/diagnostic-daemon \
$(@D)/src/diagnostic_daemon.c
endef

define DIAGNOSTIC_DAEMON_INSTALL_TARGET_CMDS
$(INSTALL) -D -m 0755 $(@D)/diagnostic-daemon \
$(TARGET_DIR)/usr/bin/diagnostic-daemon
endef

$(eval $(generic-package))
