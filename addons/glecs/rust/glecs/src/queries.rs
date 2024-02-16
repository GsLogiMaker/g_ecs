
use std::mem::MaybeUninit;

use flecs::EntityId;
use godot::engine::Script;
use godot::prelude::*;

use crate::world::_BaseGEWorld;

#[derive(Clone, Debug, Default)]
pub(crate) enum BuildType {
    #[default]
    System,
    Observer,
}

#[derive(GodotClass)]
#[class(base=RefCounted)]
pub struct _BaseSystemBuilder {
    #[base] pub(crate) base: Base<RefCounted>,
    pub(crate) world: Gd<_BaseGEWorld>,
    pub(crate) pipeline: Variant,
    pub(crate) description: flecs::ecs_query_desc_t,
    pub(crate) terms: Vec<flecs::ecs_term_t>,
    /// Describes the kind object this builder is building.
    pub(crate) build_type: BuildType,
    /// An array of the events the final observer will trigger on. Only used
    /// when building an observer.
    pub(crate) observing_events: Vec<EntityId>,
}
#[godot_api]
impl _BaseSystemBuilder {
    pub(crate) fn new(world:Gd<_BaseGEWorld>) -> Gd<Self> {
        let mut gd = Gd::from_init_fn(|base| {
            _BaseSystemBuilder {
                base,
                pipeline: Variant::nil(),
                world,
                description: unsafe { MaybeUninit::zeroed().assume_init() },
                terms: Default::default(),
                build_type: Default::default(),
                observing_events: Default::default(),
            }
        });
        gd.set_script(
            load::<Script>("res://addons/glecs/gd/system_builder.gd")
                .to_variant()
        );
        gd
    }

    #[constant]
    const INOUT_MODE_DEFAULT:flecs::ecs_inout_kind_t = flecs::ecs_inout_kind_t_EcsInOutDefault;
    #[constant]
    const INOUT_MODE_NEITHER:flecs::ecs_inout_kind_t = flecs::ecs_inout_kind_t_EcsInOutNone;
    #[constant]
    const INOUT_MODE_INOUT:flecs::ecs_inout_kind_t = flecs::ecs_inout_kind_t_EcsIn;
    #[constant]
    const INOUT_MODE_IN:flecs::ecs_inout_kind_t = flecs::ecs_inout_kind_t_EcsIn;
    #[constant]
    const INOUT_MODE_OUT:flecs::ecs_inout_kind_t = flecs::ecs_inout_kind_t_EcsIn;

    #[func]
    fn _with(
        &mut self,
        component:Gd<Script>,
        inout:flecs::ecs_inout_kind_t,
    ) -> Gd<_BaseSystemBuilder> {
        self.with_oper(component, flecs::ecs_oper_kind_t_EcsAnd);
        self.terms.last_mut().unwrap().inout = inout;
        self.to_gd()
    }

    #[func]
    fn _without(&mut self, component:Gd<Script>) -> Gd<_BaseSystemBuilder> {
        self.with_oper(component, flecs::ecs_oper_kind_t_EcsNot);
        self.to_gd()
    }

    #[func]
    fn _or_with(
        &mut self,
        component: Gd<Script>,
        inout: flecs::ecs_inout_kind_t,
    ) -> Gd<_BaseSystemBuilder> {
        self.with_oper(component, flecs::ecs_oper_kind_t_EcsOr);
        self.terms.last_mut().unwrap().inout = inout;
        self.to_gd()
    }

    #[func]
    fn _maybe_with(
        &mut self,
        component: Gd<Script>,
        inout: flecs::ecs_inout_kind_t,
    ) -> Gd<_BaseSystemBuilder> {
        todo!("Get optional terms working with system iterator first");
        self.with_oper(component, flecs::ecs_oper_kind_t_EcsOptional);
        self.terms.last_mut().unwrap().inout = inout;
        self.to_gd()
    }

    #[func]
    fn _all_from(&mut self, entity:Variant) -> Gd<_BaseSystemBuilder> {
        self.from_oper(entity, flecs::ecs_oper_kind_t_EcsAndFrom);
        self.to_gd()
    }

    #[func]
    fn _any_from(&mut self, entity:Variant) -> Gd<_BaseSystemBuilder> {
        self.from_oper(entity, flecs::ecs_oper_kind_t_EcsOrFrom);
        self.to_gd()
    }

    #[func]
    fn _none_from(&mut self, entity:Variant) -> Gd<_BaseSystemBuilder> {
        self.from_oper(entity, flecs::ecs_oper_kind_t_EcsNotFrom);
        self.to_gd()
    }

    #[func]
    fn _for_each(&mut self, callable:Callable) {
        self.on_build();
        let mut world_clone = self.world.clone();
        let mut world = world_clone.bind_mut();

        match self.build_type {
            BuildType::System => world.new_system_from_builder(self, callable),
            BuildType::Observer => world.new_observer_from_builder(self, callable),
        }
        
    }

    #[func]
    fn _set_pipeline(&mut self, pipeline:Variant) -> Gd<_BaseSystemBuilder> {
        self.pipeline = pipeline;
        let gd = self.to_gd();
        gd
    }

    fn add_term_to_buffer(&mut self, term:flecs::ecs_term_t) {
        self.terms.push(term);
    }

    fn with_oper(&mut self, component:Gd<Script>, oper:flecs::ecs_oper_kind_t) {
        // TODO: Add checks that scripts are indeed derived from components
        let comp_def = self.world
            .bind_mut()
            .get_or_add_component(&component);
        
        let term = flecs::ecs_term_t {
            id: comp_def.flecs_id,
            oper: oper,
            ..unsafe { MaybeUninit::zeroed().assume_init() }
        };

        self.add_term_to_buffer(term);
    }

    fn from_oper(&mut self, entity:Variant, oper:flecs::ecs_oper_kind_t) {
        let entity_id = self.world
            .bind_mut()
            .get_or_add_tag_entity(entity);
        
        let term = flecs::ecs_term_t {
            id: entity_id,
            oper: oper,
            ..unsafe { MaybeUninit::zeroed().assume_init() }
        };

        self.add_term_to_buffer(term);
    }

    fn on_build(&mut self) {
        self.description.filter.terms_buffer = self.terms.as_mut_ptr();
        self.description.filter.terms_buffer_count = self.terms.len() as i32;
    }
}