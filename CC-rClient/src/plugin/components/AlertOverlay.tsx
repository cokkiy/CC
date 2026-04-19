/**
 * AlertOverlay - Alert/Notification Overlay Component
 * Displays alerts and notifications on top of the UI
 */

import React, { useEffect, useState, useCallback, useMemo } from 'react';
import type { AlertConfig, AlertPanelConfig, AlertActionConfig } from './LayoutConfig';

// ============================================
// Alert Item Interface
// ============================================

interface AlertItem extends AlertPanelConfig {
  id: string;
  createdAt: number;
  expiresAt?: number;
}

// ============================================
// AlertOverlay Props
// ============================================

interface AlertOverlayProps {
  /** Alert configuration */
  config: AlertConfig;
  /** Current alerts to display */
  alerts: AlertItem[];
  /** Callback when alert is acknowledged */
  onAcknowledge?: (alertId: string) => void;
  /** Callback when alert action is clicked */
  onAction?: (alertId: string, actionId: string) => void;
  /** Callback when alert is dismissed */
  onDismiss?: (alertId: string) => void;
  /** Callback to request more alerts */
  onLoadMore?: () => void;
  /** Whether overlay is visible */
  visible?: boolean;
  /** CSS class name */
  className?: string;
}

// ============================================
// Severity Icons and Colors
// ============================================

const SEVERITY_CONFIG = {
  info: {
    icon: 'ℹ️',
    color: '#2d8cf0',
    bgColor: 'rgba(45, 140, 240, 0.1)',
    borderColor: 'rgba(45, 140, 240, 0.3)',
  },
  warning: {
    icon: '⚠️',
    color: '#c78a00',
    bgColor: 'rgba(199, 138, 0, 0.1)',
    borderColor: 'rgba(199, 138, 0, 0.3)',
  },
  error: {
    icon: '❌',
    color: '#d64545',
    bgColor: 'rgba(214, 69, 69, 0.1)',
    borderColor: 'rgba(214, 69, 69, 0.3)',
  },
  success: {
    icon: '✅',
    color: '#1f9d55',
    bgColor: 'rgba(31, 157, 85, 0.1)',
    borderColor: 'rgba(31, 157, 85, 0.3)',
  },
} as const;

// ============================================
// AlertOverlay Component
// ============================================

export const AlertOverlay: React.FC<AlertOverlayProps> = ({
  config,
  alerts,
  onAcknowledge,
  onAction,
  onDismiss,
  onLoadMore,
  visible = true,
  className = '',
}) => {
  const [dismissedIds, setDismissedIds] = useState<Set<string>>(new Set());
  const [autoHideTimers, setAutoHideTimers] = useState<Map<string, ReturnType<typeof setTimeout>>>(new Map());

  // Filter visible alerts
  const visibleAlerts = useMemo(() => {
    return alerts
      .filter(alert => !dismissedIds.has(alert.id))
      .slice(0, config.maxVisible || 5);
  }, [alerts, dismissedIds, config.maxVisible]);

  // Auto-hide functionality
  useEffect(() => {
    if (!config.autoHideAfter || config.autoHideAfter <= 0) return;

    const timers = new Map<string, ReturnType<typeof setTimeout>>();

    visibleAlerts.forEach(alert => {
      if (!alert.expiresAt && !autoHideTimers.has(alert.id)) {
        const timer = setTimeout(() => {
          handleDismiss(alert.id);
        }, config.autoHideAfter);
        timers.set(alert.id, timer);
      }
    });

    setAutoHideTimers(prev => {
      // Clear old timers
      prev.forEach((timer, id) => {
        if (!visibleAlerts.find(a => a.id === id)) {
          clearTimeout(timer);
        }
      });
      return new Map([...prev, ...timers]);
    });

    return () => {
      timers.forEach(timer => clearTimeout(timer));
    };
  }, [visibleAlerts, config.autoHideAfter]);

  // Handle acknowledge
  const handleAcknowledge = useCallback((alertId: string) => {
    onAcknowledge?.(alertId);
  }, [onAcknowledge]);

  // Handle dismiss
  const handleDismiss = useCallback((alertId: string) => {
    setDismissedIds(prev => new Set([...prev, alertId]));
    onDismiss?.(alertId);

    // Clear auto-hide timer
    const timer = autoHideTimers.get(alertId);
    if (timer) {
      clearTimeout(timer);
      setAutoHideTimers(prev => {
        const next = new Map(prev);
        next.delete(alertId);
        return next;
      });
    }
  }, [onDismiss, autoHideTimers]);

  // Handle action click
  const handleAction = useCallback((alertId: string, action: AlertActionConfig) => {
    onAction?.(alertId, action.action);
  }, [onAction]);

  // Handle load more
  const handleLoadMore = useCallback(() => {
    onLoadMore?.();
  }, [onLoadMore]);

  // If not visible or no alerts, return null
  if (!visible || !config.enabled || visibleAlerts.length === 0) {
    return null;
  }

  // Get position class
  const positionClass = `alert-overlay--${config.position || 'overlay'}`;

  return (
    <div 
      className={`alert-overlay ${positionClass} ${className}`}
      role="region"
      aria-label="Alert notifications"
    >
      <div className="alert-overlay__container">
        {visibleAlerts.map((alert, index) => (
          <AlertCard
            key={alert.id}
            alert={alert}
            index={index}
            onAcknowledge={handleAcknowledge}
            onDismiss={handleDismiss}
            onAction={handleAction}
          />
        ))}
        
        {alerts.length > (config.maxVisible || 5) && (
          <button 
            className="alert-overlay__load-more"
            onClick={handleLoadMore}
          >
            Load {alerts.length - (config.maxVisible || 5)} more alerts
          </button>
        )}
      </div>
    </div>
  );
};

// ============================================
// AlertCard Component
// ============================================

interface AlertCardProps {
  alert: AlertItem;
  index: number;
  onAcknowledge: (alertId: string) => void;
  onDismiss: (alertId: string) => void;
  onAction: (alertId: string, action: AlertActionConfig) => void;
}

const AlertCard: React.FC<AlertCardProps> = ({
  alert,
  index,
  onAcknowledge,
  onDismiss,
  onAction,
}) => {
  const [isExiting, setIsExiting] = useState(false);
  const severity = SEVERITY_CONFIG[alert.severity] || SEVERITY_CONFIG.info;
  const animationDelay = `${index * 50}ms`;

  const handleDismiss = useCallback(() => {
    setIsExiting(true);
    setTimeout(() => {
      onDismiss(alert.id);
    }, 200); // Wait for exit animation
  }, [alert.id, onDismiss]);

  const handleAcknowledge = useCallback(() => {
    onAcknowledge(alert.id);
  }, [alert.id, onAcknowledge]);

  const handleAction = useCallback((action: AlertActionConfig) => {
    onAction(alert.id, action);
  }, [alert.id, onAction]);

  return (
    <div
      className={`alert-card ${isExiting ? 'alert-card--exiting' : ''}`}
      style={{
        '--alert-color': severity.color,
        '--alert-bg': severity.bgColor,
        '--alert-border': severity.borderColor,
        animationDelay,
      } as React.CSSProperties}
      role="alert"
      aria-live={alert.severity === 'error' ? 'assertive' : 'polite'}
    >
      <div className="alert-card__icon">
        {severity.icon}
      </div>
      
      <div className="alert-card__content">
        <div className="alert-card__header">
          <h4 className="alert-card__title">{alert.title}</h4>
          {alert.timestamp && (
            <span className="alert-card__time">
              {formatTime(alert.timestamp)}
            </span>
          )}
        </div>
        
        <p className="alert-card__message">{alert.message}</p>
        
        {alert.source && (
          <div className="alert-card__source">
            Source: {alert.source}
          </div>
        )}
        
        {alert.actions && alert.actions.length > 0 && (
          <div className="alert-card__actions">
            {alert.actions.map(action => (
              <button
                key={action.id}
                className={`alert-card__action alert-card__action--${action.style || 'secondary'}`}
                onClick={() => handleAction(action)}
              >
                {action.label}
              </button>
            ))}
          </div>
        )}
      </div>
      
      <div className="alert-card__controls">
        {!alert.acknowledged && (
          <button
            className="alert-card__btn alert-card__btn--acknowledge"
            onClick={handleAcknowledge}
            title="Acknowledge"
          >
            ✓
          </button>
        )}
        <button
          className="alert-card__btn alert-card__btn--dismiss"
          onClick={handleDismiss}
          title="Dismiss"
        >
          ×
        </button>
      </div>
    </div>
  );
};

// ============================================
// AlertBadge Component (compact indicator)
// ============================================

interface AlertBadgeProps {
  count: number;
  severity?: keyof typeof SEVERITY_CONFIG;
  onClick?: () => void;
  className?: string;
}

export const AlertBadge: React.FC<AlertBadgeProps> = ({
  count,
  severity = 'warning',
  onClick,
  className = '',
}) => {
  if (count <= 0) return null;

  const severityConfig = SEVERITY_CONFIG[severity];

  return (
    <button
      className={`alert-badge ${className}`}
      onClick={onClick}
      style={{
        '--badge-color': severityConfig.color,
        '--badge-bg': severityConfig.bgColor,
      } as React.CSSProperties}
      aria-label={`${count} alerts`}
    >
      <span className="alert-badge__icon">{severityConfig.icon}</span>
      <span className="alert-badge__count">{count > 99 ? '99+' : count}</span>
    </button>
  );
};

// ============================================
// AlertToast Component (temporary notification)
// ============================================

interface AlertToastProps {
  alert: AlertPanelConfig;
  duration?: number;
  onClose?: () => void;
}

export const AlertToast: React.FC<AlertToastProps> = ({
  alert,
  duration = 5000,
  onClose,
}) => {
  const [isVisible, setIsVisible] = useState(true);
  const [isExiting, setIsExiting] = useState(false);
  const severity = SEVERITY_CONFIG[alert.severity] || SEVERITY_CONFIG.info;

  useEffect(() => {
    if (duration > 0) {
      const timer = setTimeout(() => {
        handleClose();
      }, duration);
      return () => clearTimeout(timer);
    }
  }, [duration]);

  const handleClose = () => {
    setIsExiting(true);
    setTimeout(() => {
      setIsVisible(false);
      onClose?.();
    }, 300);
  };

  if (!isVisible) return null;

  return (
    <div
      className={`alert-toast ${isExiting ? 'alert-toast--exiting' : ''}`}
      style={{
        '--alert-color': severity.color,
        '--alert-bg': severity.bgColor,
        '--alert-border': severity.borderColor,
      } as React.CSSProperties}
      role="alert"
    >
      <div className="alert-toast__icon">
        {severity.icon}
      </div>
      <div className="alert-toast__content">
        <strong className="alert-toast__title">{alert.title}</strong>
        <p className="alert-toast__message">{alert.message}</p>
      </div>
      <button 
        className="alert-toast__close"
        onClick={handleClose}
        aria-label="Close"
      >
        ×
      </button>
    </div>
  );
};

// ============================================
// AlertManager Hook
// ============================================

interface UseAlertManagerOptions {
  maxAlerts?: number;
  autoHideAfter?: number;
  position?: string;
}

interface UseAlertManagerReturn {
  alerts: AlertItem[];
  addAlert: (alert: Omit<AlertPanelConfig, 'id' | 'timestamp'>) => string;
  removeAlert: (id: string) => void;
  acknowledgeAlert: (id: string) => void;
  clearAlerts: () => void;
  unreadCount: number;
}

export function useAlertManager(options: UseAlertManagerOptions = {}): UseAlertManagerReturn {
  const { maxAlerts = 50, autoHideAfter = 0, position = 'overlay' } = options;
  const [alerts, setAlerts] = useState<AlertItem[]>([]);

  const addAlert = useCallback((alertData: Omit<AlertPanelConfig, 'id' | 'timestamp'>): string => {
    const id = `alert-${Date.now()}-${Math.random().toString(36).substr(2, 9)}`;
    const alert: AlertItem = {
      ...alertData,
      id,
      timestamp: Date.now(),
      createdAt: Date.now(),
      expiresAt: autoHideAfter > 0 ? Date.now() + autoHideAfter : undefined,
    };

    setAlerts(prev => {
      const next = [alert, ...prev];
      if (next.length > maxAlerts) {
        return next.slice(0, maxAlerts);
      }
      return next;
    });

    return id;
  }, [maxAlerts, autoHideAfter]);

  const removeAlert = useCallback((id: string) => {
    setAlerts(prev => prev.filter(a => a.id !== id));
  }, []);

  const acknowledgeAlert = useCallback((id: string) => {
    setAlerts(prev =>
      prev.map(a => (a.id === id ? { ...a, acknowledged: true } : a))
    );
  }, []);

  const clearAlerts = useCallback(() => {
    setAlerts([]);
  }, []);

  const unreadCount = useMemo(() => {
    return alerts.filter(a => !a.acknowledged).length;
  }, [alerts]);

  return {
    alerts,
    addAlert,
    removeAlert,
    acknowledgeAlert,
    clearAlerts,
    unreadCount,
  };
}

// ============================================
// Helper Functions
// ============================================

function formatTime(timestamp: number): string {
  const now = Date.now();
  const diff = now - timestamp;

  if (diff < 60000) {
    return 'Just now';
  } else if (diff < 3600000) {
    const mins = Math.floor(diff / 60000);
    return `${mins}m ago`;
  } else if (diff < 86400000) {
    const hours = Math.floor(diff / 3600000);
    return `${hours}h ago`;
  } else {
    return new Date(timestamp).toLocaleDateString();
  }
}

// ============================================
// Default Alert Configuration
// ============================================

export const DEFAULT_ALERT_CONFIG: AlertConfig = {
  enabled: true,
  position: 'overlay',
  maxVisible: 5,
  autoHideAfter: 10000,
};

export default AlertOverlay;
