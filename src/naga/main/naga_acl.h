#ifndef __NAGA_ACL_H__
#define __NAGA_ACL_H__

#define ACL_DORP(_acl) \
    (_acl).actions |= ACT_DROP

#define ACL_FWD(_acl, _port) \
{ \
    (_acl).actions |= ACT_FORWARD; \
    (_acl).frd_port = _port; \
}

#define ACL_LOG(_acl) \
    (_acl).actions |= ACT_LOG

#define ACL_CNT(_acl) \
    (_acl).actions |= ACT_COUNT

#define ACL_HIT(_acl) \
    bts_atomic64_inc((_acl).cnt)

#endif /* __NAGA_ACL_H__ */
