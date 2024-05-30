<template>
    <div>
        <Splitter>
            <SplitterPanel class="panel settings">
                <Fieldset legend="Settings">
                    <FloatLabel>
                        <InputText id="f" v-model="parameters['f']" />
                        <label for="f">Function</label>
                    </FloatLabel>
                </Fieldset>
            </SplitterPanel>
            <SplitterPanel class="panel">
                <Fieldset legend="Output">
                    <img :src="imageSource" alt="">
                </Fieldset>
                <Fieldset legend="Code">
                    <code>
                        {{ JSON.stringify(parameters, null, 4) }}
                    </code>

                    <br>
                    <br>

                    <Button>Copy Embed Link</Button>
                </Fieldset>
            </SplitterPanel>
        </Splitter>
    </div>
</template>

<script setup lang="ts">
import Splitter from "primevue/splitter";
import SplitterPanel from "primevue/splitterpanel";
import Fieldset from "primevue/fieldset";
import FloatLabel from "primevue/floatlabel";
import InputText from "primevue/inputtext";
import Button from "primevue/button";
import {computed, ref} from "vue";

interface UnlogicParameters {
    f: string,
}

const parameters = ref<UnlogicParameters>({ f: 'x^3;' });

const imageSource = computed(() => {
    return `https://unlogic.seymour.global/render/${btoa(JSON.stringify(parameters.value))}`;
});
</script>

<style scoped lang="scss">
.panel {
    padding: 1em;
}

img {
    max-width: 100%;
}
</style>