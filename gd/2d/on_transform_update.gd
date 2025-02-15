
## An event that is emitted whenever the transform of an entity changes.
##
## Main purpose is to prevent infinte loops for components
## that need to both write and listen for changes to transform.

class_name GFOnTransformUpdate extends GFTag
