<!-- src/components/ProtectedRoute.svelte -->
<script>
  import { onMount } from "svelte";
  import { isAuthenticated, checkAuth } from "../authStore";
  import { navigate } from "svelte-routing";

  export let component;

  onMount(async () => {
    await checkAuth();
    if (!$isAuthenticated) {
      navigate("/login");
    }
  });
</script>

{#if $isAuthenticated}
  <svelte:component this={component} />
{:else}
  <p>Loading...</p>
{/if}

